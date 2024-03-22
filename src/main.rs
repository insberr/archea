#![forbid(unsafe_code)]

//! How to use an external thread to run an infinite task and communicate with a channel.

use crate::engine::systems::chunk::{Chunks, Pixel, PixelType};
use std::collections::BTreeMap;
use std::f32::consts::PI;
use std::fmt::Formatter;
use std::ops::Mul;
use bevy::prelude::*;
// Using crossbeam_channel instead of std as std `Receiver` is `!Sync`
use crossbeam_channel::{bounded, Receiver};
use rand::{Rng, SeedableRng};
use std::time::{Duration, Instant};
use bevy::pbr::{CascadeShadowConfigBuilder, OpaqueRendererMethod};
use bevy::render::camera::{Exposure, PhysicalCameraParameters};
use bevy::render::render_resource::Face;
use bevy::render::view::NoFrustumCulling;
use bevy_flycam::{FlyCam, NoCameraPlayerPlugin, PlayerPlugin};
use bevy_panorbit_camera::{PanOrbitCamera, PanOrbitCameraPlugin};
use bytemuck::{Pod, Zeroable};
use rand::prelude::ThreadRng;

mod engine;
use engine::stuff::vect3::*;
use crate::engine::plugins::instancing::{InstancingPlugin, InstanceData, InstancedMaterial};
use crate::engine::systems::movement::update_pixel_positions;
use crate::engine::systems::temperature::update_pixel_temperatures;

#[derive(Component, Copy, Clone, Pod, Zeroable)]
#[repr(C)]
pub struct CubePixel {
    position: Vec3,
    scale: f32,
    color: [f32; 4],
}

impl InstancedMaterial for CubePixel {
    type M = StandardMaterial;

    fn shader_path() -> &'static str {
        "shaders/instancing2.wgsl"
    }
}

fn main() {
    App::new()
        // Stream used to send the pixels data from the tread to the game
        .add_event::<StreamEventPixelPositions>()
        .add_event::<StreamEventChunks>()

        .insert_resource(Parameters(PhysicalCameraParameters {
            aperture_f_stops: 1.0,
            shutter_speed_s: 1.0 / 125.0,
            sensitivity_iso: 100.0,
        }))

        .add_plugins(DefaultPlugins)
        .add_plugins(InstancingPlugin::<CubePixel>::default())

        // Camera Plugin
        // .add_plugins(PanOrbitCameraPlugin)
        .add_plugins(NoCameraPlayerPlugin)

        .add_systems(Startup, setup)
        .add_systems(Update, (read_stream, update_instancing))
        .run();
}

// #[derive(Clone)]
// struct Pixel {
//     pixel_type: PixelType,
//     pixel_temperature: f32,
//     dont_move: bool,
// }

#[derive(Clone)]
struct PixelPositions {
    map: BTreeMap<Vect3, Pixel>,
    is_map_dirty: bool,
    is_colors_dirty: bool,
}

/* Stream Structs */
#[derive(Resource, Deref)]
struct StreamReceiverPixelPositions(Receiver<PixelPositions>);

#[derive(Event)]
struct StreamEventPixelPositions(PixelPositions);

#[derive(Resource, Deref)]
struct StreamReceiverChunks(Receiver<Chunks>);

#[derive(Event)]
struct StreamEventChunks(Chunks);

// Camera thingy
#[derive(Resource, Default, Deref, DerefMut)]
struct Parameters(PhysicalCameraParameters);

// Setup
fn setup(
    parameters: Res<Parameters>,
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
) {
    // Spawn Light
    commands.spawn(PointLightBundle {
        point_light: PointLight {
            shadows_enabled: true,
            intensity: 90_000_000.,
            range: 1000.0,
            color: Color::WHITE,
            ..default()
        },
        transform: Transform::from_xyz(20.0, 50.0, 0.0).with_rotation(Quat::from_rotation_x((0.5 * PI) / 180.0)),
        ..default()
    });

    // Create the ground plane
    commands.spawn(PbrBundle {
        mesh: meshes.add(Plane3d::default().mesh().size(10000.0, 10000.0)),
        material: materials.add(Color::GRAY),
        transform: Transform::from_xyz(0.0, -1.0, 0.0),
        ..default()
    });

    // Spawn Camera
    commands.spawn((
        Camera3dBundle {
            transform: Transform::from_translation(Vec3::new(60.0, 60.0, 60.0)),
            exposure: Exposure::from_physical_camera(**parameters),
            ..default()
        },
        // PanOrbitCamera::default(),
        FlyCam,
    ));

    let (tx, rx) = bounded::<PixelPositions>(1);
    let (sender_chunks, receiver_chunks) = bounded::<Chunks>(1);

    std::thread::spawn(move || {
        // let mut rng = StdRng::seed_from_u64(19878367467713);
        let mut rng: ThreadRng = rand::thread_rng();
        let mut pixel_positions = PixelPositions {
            map: Default::default(),
            is_map_dirty: false,
            is_colors_dirty: false,
        };

        let mut chunks = Chunks::new(Vect3i::new(1, 1, 1), Vect3i::new(20, 50, 20));

        create_pixels(&mut pixel_positions.map);

        for (pos, pix) in pixel_positions.map.iter() {
            chunks.create_pixel(*pos, pix.clone().pixel_type);
        }

        // chunks.print_chunks();

        loop {
            pixel_positions.is_map_dirty = false;
            // let start_time = Instant::now();
            let duration = Duration::from_secs_f32(0.2);
            std::thread::sleep(duration);

            // update_pixel_positions(&mut pixel_positions, &mut rng);
            // if pixel_positions.is_map_dirty {
            //     tx.send(pixel_positions.clone()).unwrap();
            // }

            // update_pixel_temperatures(&mut pixel_positions, &mut rng);
            // if pixel_positions.is_map_dirty {
            //     tx.send(pixel_positions.clone()).unwrap();
            // }

            chunks.update_chunks();
            // chunks.update()
            // if chunks is dirty, also somehow only rerender the dirty ones
            sender_chunks.send(chunks.clone()).unwrap();
        }
    });

    commands.insert_resource(StreamReceiverPixelPositions(rx));
    commands.insert_resource(StreamReceiverChunks(receiver_chunks));
}

fn color_for(pixel: &Pixel) -> Color {
    match pixel.pixel_type {
        PixelType::Sand => Color::ORANGE,
        PixelType::Water => Color::rgba(0.0, 0.2, 0.9, 0.4),
        PixelType::Lava => Color::ORANGE_RED,
        PixelType::Steam => Color::WHITE,
        PixelType::Rock => Color::GRAY,

        PixelType::Invalid | _ => Color::PURPLE,
    }
}

// This system reads from the receiver and sends events to Bevy
fn read_stream(
    receiver_pixel_positions: Res<StreamReceiverPixelPositions>,
    mut events_pixel_positions: EventWriter<StreamEventPixelPositions>,
    receiver_chunks: Res<StreamReceiverChunks>,
    mut events_chunks: EventWriter<StreamEventChunks>
) {
    for from_stream in receiver_pixel_positions.try_iter() {
        events_pixel_positions.send(StreamEventPixelPositions(from_stream));
    }

    for from_stream in receiver_chunks.try_iter() {
        events_chunks.send(StreamEventChunks(from_stream));
    }
}

#[derive(Component)]
struct ChunkDebugRender;

fn update_instancing(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
    mut reader: EventReader<StreamEventPixelPositions>,
    mut reader_chunks: EventReader<StreamEventChunks>,
    instance_entities: Query<Entity, With<InstanceData<CubePixel>>>,
    chunk_debug_entities: Query<Entity, With<ChunkDebugRender>>
) {
    let material = materials.add(StandardMaterial {
        base_color: Color::WHITE,
        double_sided: true,
        alpha_mode: AlphaMode::Blend, // This does absolutely nothing ...
        ..default()
    });

    for (per_frame, chunks) in reader_chunks.read().enumerate() {
        for chunk_ent in chunk_debug_entities.iter() {
            commands.entity(chunk_ent).despawn();
        }
        for instance in instance_entities.iter(){
            commands.entity(instance).despawn();
        }
        for (index, (chunk_pos, chunk)) in chunks.0.chunks.iter().enumerate() {
            let chunk_world_pos = *chunk_pos * chunk.bounds;
            commands.spawn((
                PbrBundle {
                    mesh: meshes.add(Cuboid::new(chunk.bounds.x as f32,chunk.bounds.y as f32,chunk.bounds.z as f32)),
                    material: materials.add(*Color::WHITE.set_a(0.2)),
                    transform: Transform::from_xyz(chunk_world_pos.x as f32, chunk_world_pos.y as f32, chunk_world_pos.z as f32),
                    ..default()
                },
                ChunkDebugRender
            ));

            let mesh = meshes.add(Cuboid::new(1.0, 1.0, 1.0));
            commands.spawn((
                mesh.clone(),
                material.clone(),
                SpatialBundle {
                    // This is needed otherwise nothing renders I think
                    transform: Transform::from_xyz(chunk_world_pos.x as f32, f32::MIN, chunk_world_pos.y as f32),
                    ..SpatialBundle::INHERITED_IDENTITY
                },
                InstanceData::<CubePixel> {
                    data: chunk.pixels
                        .iter()
                        .map(|(pos, pix)| CubePixel {
                            position: pos.to_vec3(),
                            scale: 1.0,
                            // temporary
                            color: color_for(pix).as_rgba_f32(),
                        })
                        .collect(),
                    mesh: mesh.clone()
                },
                NoFrustumCulling,
            ));
        }
    }
    // for (per_frame, event) in reader.read().enumerate() {
    //     for instance in instance_entities.iter(){
    //         commands.entity(instance).despawn();
    //     }
    //     // todo: There should only be one thing to read in the reader... Check for that later
    //     // commands.spawn(
    //     //     // meshes.add(Cuboid::new(1.0, 1.0, 1.0)),
    //     //     // SpatialBundle::INHERITED_IDENTITY,
    //     //     // InstancedMaterial(
    //     //     //     event.0.map.iter()
    //     //     //         .map(|(position, data)| InstanceData {
    //     //     //             position: position.to_vec3(),
    //     //     //             scale: 1.0,
    //     //     //             temp: data.pixel_temperature,
    //     //     //             color: color_for(&data).as_rgba_f32(), //Color::RED.as_rgba_f32(),
    //     //     //         })
    //     //     //         .collect(),
    //     //     // ),
    //     //     // NoFrustumCulling,
    //     //     InstanceData::<CubePixel> {
    //     //         mesh: meshes.add(Cuboid::new(1.0, 1.0, 1.0)),
    //     //         data: vec![CubePixel { position: Vec3::new(0.0, 0.0, 0.0), scale: 1.0 }],
    //     //     }
    //     // );
    //
    //     let mesh = meshes.add(Cuboid::new(1.0, 1.0, 1.0));
    //     let material = materials.add(StandardMaterial {
    //         base_color: Color::WHITE,
    //         double_sided: true,
    //         alpha_mode: AlphaMode::Blend, // This does absolutely nothing ...
    //         ..default()
    //     });
    //
    //     commands.spawn((
    //         mesh.clone(),
    //         material.clone(),
    //         SpatialBundle {
    //             // This is needed otherwise nothing renders I think
    //             transform: Transform::from_xyz(0., f32::MIN, 0.),
    //             ..SpatialBundle::INHERITED_IDENTITY
    //         },
    //         InstanceData::<CubePixel> {
    //             data: event.0.map
    //                 .iter()
    //                 .map(|(pos, pix)| CubePixel {
    //                     position: pos.to_vec3(),
    //                     scale: 1.0,
    //                     // temporary
    //                     color: color_for(&pix).as_rgba_f32(),
    //                 })
    //                 .collect(),
    //             mesh: mesh.clone(),
    //         },
    //         NoFrustumCulling,
    //     ));
    // }
}

// Create the pixels on startup
fn create_pixels(mut pixels: &mut BTreeMap<Vect3, Pixel>) {
    for y in 10..=30 {
        for x in -25..=25 {
            for z in -25..=25 {
                // pixels.insert(Vect3::new(x as f32, y as f32, z as f32), Pixel {
                //     pixel_type: PixelType::Lava,
                //     dont_move: false,
                //     temperature: 100.0,
                //     weight: 0.0,
                // });
                if y % 3 == 0 {
                    pixels.insert(Vect3::new(x as f32, y as f32, z as f32), Pixel {
                        pixel_type: PixelType::Sand,
                        dont_move: false,
                        temperature: 60.0,
                        weight: 0.0,
                    });
                } else if y % 2 == 0 {
                    pixels.insert(Vect3::new(x as f32, y as f32, z as f32), Pixel {
                        pixel_type: PixelType::Water,
                        dont_move: false,
                        temperature: 60.0,
                        weight: 0.0,
                    });
                } else {
                    pixels.insert(Vect3::new(x as f32, y as f32, z as f32), Pixel {
                        pixel_type: PixelType::Lava,
                        dont_move: false,
                        temperature: 60.0,
                        weight: 0.0,
                    });
                }
            }
        }
    }
}