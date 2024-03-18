#![forbid(unsafe_code)]

use bevy::prelude::*;
use bevy::render::render_phase::AddRenderCommand;
use bevy::render::view::NoFrustumCulling;

use bevy_mod_picking::prelude::*;
use bevy_panorbit_camera::{PanOrbitCamera, PanOrbitCameraPlugin};

use std::collections::BTreeMap;
use std::f32::consts::PI;

// Sand Code
mod engine;
use engine::plugins::instancing::{CustomMaterialPlugin, InstanceData, InstanceMaterialData};

use engine::systems::lava_movement::*;
use engine::systems::sand_movement::*;
use engine::systems::water_movement::*;
use crate::engine::stuff::vect3::Vect3;

#[derive(PartialEq, Clone)]
enum PixelType {
    Unmovable = -2,
    Invalid = -1,
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


#[derive(Resource, Default)]
struct PixelPositions {
    map: BTreeMap<Vect3, Pixel>,
    is_map_dirty: bool,
    is_colors_dirty: bool,
}

fn main() {
    App::new()
        .add_plugins(DefaultPlugins)

        .add_plugins(CustomMaterialPlugin)

        .add_plugins(PanOrbitCameraPlugin)
        .add_plugins(DefaultPickingPlugins)
        .insert_resource(DebugPickingMode::Normal)

        .init_resource::<PixelPositions>()
        .insert_resource(Time::<Fixed>::from_seconds(0.1))

        .add_systems(Startup, (setup, create_pixels).chain())

        .add_systems(FixedUpdate, sand_movement)
        .add_systems(FixedUpdate, water_movement)
        .add_systems(FixedUpdate, lava_movement)

        .add_systems(FixedPostUpdate, update_render_pixels)

        .run();
}

fn update_render_pixels(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
    mut pixels: ResMut<PixelPositions>,
    sad: Query<Entity, With<InstanceMaterialData>>
) {
    // let mut sand = 0;
    // let mut water = 0;
    // for p in pixels.map.iter() {
    //     match p.1.pixel_type {
    //         PixelType::Sand => {
    //             sand += 1;
    //         },
    //         PixelType::Water => {
    //             water += 1;
    //         },
    //         _ => {}
    //     }
    // }
    // println!("Sand: {sand} | Water: {water}");

    if pixels.is_map_dirty == false {
        return;
    }

    // let shapes = [
    //     meshes.add(Cuboid {
    //         half_size: Vec3 {
    //             x: 0.5,
    //             y: 0.5,
    //             z: 0.5,
    //         }
    //     }),
    // ];
    //
    // // despawn all
    // for parent in &parents_query {
    //     // Pull out the Pixel component
    //     commands.entity(parent).despawn();
    // }
    //
    // for (pos_index, pix) in pixels.map.iter() {
    //     let pos = *pos_index; // Vect3::from_index(*pos_index);
    //     spawn_cube(&mut commands, shapes[0].clone(), &mut materials, *pos.x, *pos.y, *pos.z, pix.pixel_type.clone());
    // }

    // for (transform, material) in render_transform_query.iter() {
    // // render_transform_query
    // //     .par_iter_mut()
    // //     .batching_strategy(BatchingStrategy::fixed(32))
    // //     .for_each(|(transform, material)| {
    //         // if let Ok((transform, material)) = render_transform_query.get_mut(entity) {
    //         let key = Vect3::from_vec3(transform.translation);
    //         let mut color = Color::NONE;
    //
    //         // If there's a pixel at the position
    //         if let Some(map_value) = pixels.map.get(&key) {
    //             color = match &map_value.pixel_type {
    //                 PixelType::Invalid => Color::GREEN, // HOW
    //                 PixelType::Sand => Color::ORANGE,
    //                 PixelType::Water => {
    //                     let mut c = Color::BLUE;
    //                     c.set_a(0.4);
    //                     c
    //                 },
    //                 _ => Color::NONE,
    //             };
    //             // materials.get_mut(material).unwrap().base_color = color;
    //         }
    //
    //         // pix.pixel_type = map_value.clone().pixel_type;
    //         // No exist, change color to clear
    //         // pix.pixel_type = PixelType::Invalid;
    //         materials.get_mut(material).unwrap().base_color = color;
    //         // pixels.is_colors_dirty = true;
    //
    //         // transform.rotate_z(-PI / 2. * time.delta_seconds());
    //         // if (transform.translation.y < 0.0) {
    //         // if pixel.destroy {
    //         //     commands.entity(parent).despawn();
    //         // }
    //     // }
    // }

    for instance in sad.iter(){
        commands.entity(instance).despawn();
    }

    commands.spawn((
        meshes.add(Cuboid::new(1.0, 1.0, 1.0)),
        SpatialBundle::INHERITED_IDENTITY,
        InstanceMaterialData(
            pixels.map.iter()
                .map(|(position, data)| InstanceData {
                    position: position.to_vec3(),
                    scale: 1.0,
                    color: color_for(&data).as_rgba_f32(), //Color::RED.as_rgba_f32(),
                })
                .collect(),
        ),
        // NOTE: Frustum culling is done based on the Aabb of the Mesh and the GlobalTransform.
        // As the cube is at the origin, if its Aabb moves outside the view frustum, all the
        // instanced cubes will be culled.
        // The InstanceMaterialData contains the 'GlobalTransform' information for this custom
        // instancing, and that is not taken into account with the built-in frustum culling.
        // We must disable the built-in frustum culling by adding the `NoFrustumCulling` marker
        // component to avoid incorrect culling.
        NoFrustumCulling,
    ));

    pixels.is_map_dirty = false;
    // pixels = pixels;
}

// fn update_transforms_list(
//     mut pixel_transforms: ResMut<PixelPositions>,
//     pixels: Query<(&Transform, &RenderPixel)>
// ) {
//     // pixel_transforms.positions.clear();
//     pixel_transforms.map.clear();
//     for (tr, pix) in pixels.iter() {
//         pixel_transforms.map.insert(Vect3::from_vec3(tr.translation).to_index(), Pixel {
//             pixel_temperature: 0.0,
//             dont_move: false,
//             pixel_type: PixelType::Water,
//         });
//         // pixel_transforms.positions.push(PixelTransform {
//         //     transform: tr.clone(),
//         //     entity: pix.id,
//         // });
//     }
//
//     // for (k, v) in pixel_transforms.map.iter() {
//     //     // let v_t = &v.pixel_type;
//     //     let v_tr = &v.transform;
//     //     let c = k & 0x000000FFu128;
//     //     let b = (k >> 32) & 0x000000FFu128;
//     //     let a = k >> 64;
//     //     // a = (int)(c & 0xFFFFFFFFUL);
//     //     // b = (int)(c >> 32);
//     //     println!("Map: [{a} {b} {c}] : {v_tr}");
//     // }
//     return;
// }

fn check_destroy(
    mut commands: Commands,
    parents_query: Query<Entity, With<RenderPixel>>,
    mut pixel_query: Query<&mut RenderPixel>,
) {
    // For each entity with the Pixel component
    for parent in &parents_query {
        // Pull out the Pixel component
        if let Ok(pixel) = pixel_query.get_mut(parent) {
            // transform.rotate_z(-PI / 2. * time.delta_seconds());
            // if (transform.translation.y < 0.0) {
            if pixel.destroy {
                commands.entity(parent).despawn();
            }
        }
    }
}

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

fn spawn_cube(
    commands: &mut Commands,
    shape: Handle<Mesh>,
    materials: &mut ResMut<Assets<StandardMaterial>>,
    x: f32,
    y: f32,
    z: f32,
    pixel_type: PixelType,
) {
    // let mut rng = rand::thread_rng();

   commands
        .spawn((PbrBundle {
            mesh: shape,
            material: materials.add(Color::NONE),
            transform: Transform::from_xyz(
                x,//rng.gen_range(-50..=50) as f32,
                y,//10.0,
                z//rng.gen_range(-50..=50) as f32,
            ),
            ..default()
        },RenderPixel {
            dont_move: false,
            pixel_type,
            destroy: false
        }))
        // .insert(Pixel {
        //     dont_move: false,
        //     pixel_type: pixel_type,
        //     destroy: false,
        // })
        // .insert(PixelSand)
        // Despawn an entity when clicked:
        .insert(On::<Pointer<Click>>::target_commands_mut(|_click, target_commands| {
            target_commands.despawn();
        }))
        // Optional: adds selection, highlighting, and helper components.
        .insert(PickableBundle::default());
}

fn setup(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
    _pixels: ResMut<PixelPositions>
) {
    commands.insert_resource(PixelPositions {
        map: BTreeMap::new(),
        is_map_dirty: false,
        is_colors_dirty: false,
    });

    // let shape = meshes.add(Cuboid {
    //     half_size: Vec3 {
    //         x: 0.5,
    //         y: 0.5,
    //         z: 0.5,
    //     }
    // });
    //
    // commands
    //     .spawn((PbrBundle {
    //         mesh: shape.clone(),
    //         material: materials.add(Color::RED),
    //         transform: Transform::from_xyz(
    //             50.0,//rng.gen_range(-50..=50) as f32,
    //             0.0,//10.0,
    //             50.0//rng.gen_range(-50..=50) as f32,
    //         ),
    //         ..default()
    //     },RenderPixel {
    //         dont_move: false,
    //         pixel_type: PixelType::Water,
    //         destroy: false
    //     }))
    //     // .insert(Pixel {
    //     //     dont_move: false,
    //     //     pixel_type: pixel_type,
    //     //     destroy: false,
    //     // })
    //     // .insert(PixelSand)
    //     // Despawn an entity when clicked:
    //     .insert(On::<Pointer<Click>>::target_commands_mut(|_click, target_commands| {
    //         target_commands.despawn();
    //     }))
    //     // Optional: adds selection, highlighting, and helper components.
    //     .insert(PickableBundle::default());
    // commands
    //     .spawn((PbrBundle {
    //         mesh: shape.clone(),
    //         material: materials.add(Color::rgba(1.0, 0.0, 0.0, 0.2)),
    //         transform: Transform::from_xyz(
    //             50.0,//rng.gen_range(-50..=50) as f32,
    //             0.0,//10.0,
    //             -50.0//rng.gen_range(-50..=50) as f32,
    //         ),
    //         ..default()
    //     },RenderPixel {
    //         dont_move: false,
    //         pixel_type: PixelType::Water,
    //         destroy: false
    //     }))
    //     // .insert(Pixel {
    //     //     dont_move: false,
    //     //     pixel_type: pixel_type,
    //     //     destroy: false,
    //     // })
    //     // .insert(PixelSand)
    //     // Despawn an entity when clicked:
    //     .insert(On::<Pointer<Click>>::target_commands_mut(|_click, target_commands| {
    //         target_commands.despawn();
    //     }))
    //     // Optional: adds selection, highlighting, and helper components.
    //     .insert(PickableBundle::default());

    // Create the light
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

    // let mut trs = vec![];
    // for y in 0..=20 {
    //     for x in 0..=20 {
    //         for z in 0..=20 {
    //             trs.push((PbrBundle {
    //                 mesh: shape.clone(),
    //                 material: materials.add(Color::NONE),
    //                 transform: Transform::from_xyz(
    //                     x as f32,
    //                     y as f32,
    //                     z as f32,
    //                 ),
    //                 ..default()
    //             },RenderPixel {
    //                 dont_move: false,
    //                 pixel_type: PixelType::Invalid,
    //                 destroy: false
    //             }));
    //         }
    //     }
    // }

    // commands.spawn_batch(trs);
        // .insert(On::<Pointer<Click>>::target_commands_mut(|_click, target_commands| {
        //     target_commands.despawn();
        // }))
        // // Optional: adds selection, highlighting, and helper components.
        // .insert(PickableBundle::default());

    // commands
    //     .spawn((PbrBundle {
    //         mesh: shape.clone(),
    //         material: materials.add(Color::NONE),
    //         transform: trs[0],
    //         ..default()
    //     },RenderPixel {
    //         dont_move: false,
    //         pixel_type: PixelType::Invalid,
    //         destroy: false
    //     }))
    //     .insert(On::<Pointer<Click>>::target_commands_mut(|_click, target_commands| {
    //         target_commands.despawn();
    //     }))
    //     // Optional: adds selection, highlighting, and helper components.
    //     .insert(PickableBundle::default());

    // directional 'sun' light
    // commands.spawn(DirectionalLightBundle {
    //     directional_light: DirectionalLight {
    //         illuminance: light_consts::lux::CLEAR_SUNRISE,
    //         shadows_enabled: true,
    //         ..default()
    //     },
    //     transform: Transform {
    //         translation: Vec3::new(0.0, 50.0, 0.0),
    //         rotation: Quat::from_rotation_x(-PI / 4.),
    //         ..default()
    //     },
    //     // The default cascade config is designed to handle large scenes.
    //     // As this example has a much smaller world, we can tighten the shadow
    //     // bounds for better visual quality.
    //     cascade_shadow_config: CascadeShadowConfigBuilder {
    //         first_cascade_far_bound: 4.0,
    //         maximum_distance: 10.0,
    //         ..default()
    //     }
    //         .into(),
    //     ..default()
    // });

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
        // Really???
        _ => Color::PURPLE,
    }
}

fn create_pixels(
    mut pixels: ResMut<PixelPositions>
) {
    for y in 10..=40 {
        for x in -15..=15 {
            for z in -15..=15 {
                if y % 3 == 0 {
                // let entity = spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32, PixelType::Sand);
                // commands.entity(entity);
                    pixels.map.insert(Vect3::new(x as f32, y as f32, z as f32)/*.to_index()*/, Pixel {
                        pixel_type: PixelType::Sand,
                        dont_move: false,
                        pixel_temperature: 0.0,
                    });
                } else if y % 2 == 0 {
                    pixels.map.insert(Vect3::new(x as f32, y as f32, z as f32)/*.to_index()*/, Pixel {
                        pixel_type: PixelType::Water,
                        dont_move: false,
                        pixel_temperature: 0.0,
                    });
                } else {
                    pixels.map.insert(Vect3::new(x as f32, y as f32, z as f32)/*.to_index()*/, Pixel {
                        pixel_type: PixelType::Lava,
                        dont_move: false,
                        pixel_temperature: 0.0,
                    });
                }
                //     // let entity = spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32, PixelType::Water);
                //     // commands.entity(entity);
                //     pixels.map.insert(Vect3::new(a as f32, b as f32, c as f32).to_index(), Pixel {
                //         pixel_type: PixelType::Water,
                //         dont_move: false,
                //         pixel_temperature: 0.0,
                //     });
                // }
            }
        }
    }



    pixels = pixels;
}