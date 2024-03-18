#![forbid(unsafe_code)]

//! How to use an external thread to run an infinite task and communicate with a channel.

use std::collections::BTreeMap;
use std::f32::consts::PI;
use bevy::prelude::*;
// Using crossbeam_channel instead of std as std `Receiver` is `!Sync`
use crossbeam_channel::{bounded, Receiver};
use rand::{Rng, SeedableRng};
use std::time::{Duration, Instant};
use bevy::render::view::NoFrustumCulling;
use bevy_panorbit_camera::{PanOrbitCamera, PanOrbitCameraPlugin};
use rand::prelude::ThreadRng;

mod engine;
use engine::stuff::vect3::*;
use crate::engine::plugins::instancing::{CustomMaterialPlugin, InstanceData, InstanceMaterialData};
use crate::engine::systems::movement::update_pixel_positions;

fn main() {
    App::new()
        .add_event::<StreamEvent>()
        .add_plugins(DefaultPlugins)
        .add_plugins(CustomMaterialPlugin)
        .add_plugins(PanOrbitCameraPlugin)
        .add_systems(Startup, setup)
        .add_systems(Update, (read_stream, spawn_text, move_text))
        .run();
}


#[derive(PartialEq, Clone)]
enum PixelType {
    Invalid = -1,
    Unmovable = 0,
    Sand,
    Water,
    Lava,
    Steam,
    Rock,
}

#[derive(Clone)]
struct Pixel {
    pixel_type: PixelType,
    pixel_temperature: f32,
    dont_move: bool,
}

#[derive(Clone)]
struct PixelPositions {
    map: BTreeMap<Vect3, Pixel>,
    is_map_dirty: bool,
    is_colors_dirty: bool,
}

// RenderPixel only exists because I am too lazy to remove it
#[derive(Component)]
struct RenderPixel {
    dont_move: bool,
    destroy: bool,
    pixel_type: PixelType,
}
impl RenderPixel {
    pub fn destroy(&mut self) {
        self.destroy = false;
    }
}

#[derive(Resource, Deref)]
struct StreamReceiver(Receiver<PixelPositions>);

#[derive(Event)]
struct StreamEvent(PixelPositions);

fn setup(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
) {
    // Spawn Light
    commands.spawn(PointLightBundle {
        point_light: PointLight {
            shadows_enabled: true,
            intensity: 90_000_000.,
            range: 100.0,
            color: Color::WHITE,
            ..default()
        },
        transform: Transform::from_xyz(20.0, 50.0, 0.0).with_rotation(Quat::from_rotation_x((0.5 * PI) / 180.0)),
        ..default()
    });

    // Create the ground plane
    commands.spawn(PbrBundle {
        mesh: meshes.add(Plane3d::default().mesh().size(100.0, 100.0)),
        material: materials.add(Color::WHITE),
        transform: Transform::from_xyz(0.0, -0.5, 0.0),
        ..default()
    });

    // Spawn Camera
    commands.spawn((
        Camera3dBundle {
            transform: Transform::from_translation(Vec3::new(60.0, 60.0, 60.0)),
            ..default()
        },
        PanOrbitCamera::default(),
    ));

    let (tx, rx) = bounded::<PixelPositions>(1);
    std::thread::spawn(move || {
        // let mut rng = StdRng::seed_from_u64(19878367467713);
        let mut rng: ThreadRng = rand::thread_rng();
        let mut pixel_positions = PixelPositions {
            map: Default::default(),
            is_map_dirty: false,
            is_colors_dirty: false,
        };

        create_pixels(&mut pixel_positions.map);

        loop {
            pixel_positions.is_map_dirty = false;
            // let start_time = Instant::now();
            let duration = Duration::from_secs_f32(0.01);
            std::thread::sleep(duration);

            update_pixel_positions(&mut pixel_positions, &mut rng);

            if pixel_positions.is_map_dirty {
                tx.send(pixel_positions.clone()).unwrap();
            }
        }
    });

    commands.insert_resource(StreamReceiver(rx));
}

fn color_for(pixel: &Pixel) -> Color {
    match pixel.pixel_type {
        PixelType::Invalid => Color::PURPLE, // HOW
        PixelType::Sand => Color::ORANGE,
        PixelType::Water => {
            Color::rgba(0.0, 0.2, 0.9, 0.4)
            // c.set_a(0.4);
            // c
        },
        PixelType::Lava => Color::ORANGE_RED,
        PixelType::Steam => Color::WHITE,
        PixelType::Rock => Color::GRAY,
        // Really???
        _ => Color::PURPLE,
    }
}

// This system reads from the receiver and sends events to Bevy
fn read_stream(receiver: Res<StreamReceiver>, mut events: EventWriter<StreamEvent>) {
    for from_stream in receiver.try_iter() {
        events.send(StreamEvent(from_stream));
    }
}

fn spawn_text(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut reader: EventReader<StreamEvent>,
    instance_entities: Query<Entity, With<InstanceMaterialData>>
) {
    // let text_style = TextStyle {
    //     font_size: 20.0,
    //     ..default()
    // };
    //
    // for (per_frame, event) in reader.read().enumerate() {
    //     commands.spawn(Text2dBundle {
    //         text: Text::from_section(event.0.to_string(), text_style.clone())
    //             .with_justify(JustifyText::Center),
    //         transform: Transform::from_xyz(per_frame as f32 * 100.0, 300.0, 0.0),
    //         ..default()
    //     });
    // }




    for (per_frame, event) in reader.read().enumerate() {
        // println!("Update render");

        for instance in instance_entities.iter(){
            commands.entity(instance).despawn();
        }
        // todo: There should only be one thing to read in the reader... Check for that later
        commands.spawn((
            meshes.add(Cuboid::new(1.0, 1.0, 1.0)),
            SpatialBundle::INHERITED_IDENTITY,
            InstanceMaterialData(
                event.0.map.iter()
                    .map(|(position, data)| InstanceData {
                        position: position.to_vec3(),
                        scale: 1.0,
                        temp: data.pixel_temperature,
                        color: color_for(&data).as_rgba_f32(), //Color::RED.as_rgba_f32(),
                    })
                    .collect(),
            ),
            NoFrustumCulling,
        ));
    }

    // todo: Mark no longer dirty
}

fn move_text(
    mut commands: Commands,
    mut texts: Query<(Entity, &mut Transform), With<Text>>,
    time: Res<Time>,
) {
    for (entity, mut position) in &mut texts {
        position.translation -= Vec3::new(0.0, 100.0 * time.delta_seconds(), 0.0);
        if position.translation.y < -300.0 {
            commands.entity(entity).despawn();
        }
    }
}

fn create_pixels(mut pixels: &mut BTreeMap<Vect3, Pixel>) {
    println!("Create my pixels!");
    for y in 10..=40 {
        for x in -15..=15 {
            for z in -15..=15 {
                if y % 3 == 0 {
                    pixels.insert(Vect3::new(x as f32, y as f32, z as f32), Pixel {
                        pixel_type: PixelType::Sand,
                        dont_move: false,
                        pixel_temperature: 60.0,
                    });
                } else if y % 2 == 0 {
                    pixels.insert(Vect3::new(x as f32, y as f32, z as f32), Pixel {
                        pixel_type: PixelType::Water,
                        dont_move: false,
                        pixel_temperature: 60.0,
                    });
                } else {
                    pixels.insert(Vect3::new(x as f32, y as f32, z as f32), Pixel {
                        pixel_type: PixelType::Lava,
                        dont_move: false,
                        pixel_temperature: 10_000.0,
                    });
                }
            }
        }
    }
}