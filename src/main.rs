#![forbid(unsafe_code)]

use std::collections::BTreeMap;
use std::f32::consts::PI;
use std::ops;
use bevy::ecs::query::BatchingStrategy;

// use rand::prelude::*;
use bevy::prelude::*;
use bevy::render::Render;
use bevy::render::render_phase::AddRenderCommand;
use bevy::render::view::NoFrustumCulling;
use bevy_mod_picking::prelude::*;
use bevy_panorbit_camera::{PanOrbitCamera, PanOrbitCameraPlugin};
use ordered_float::OrderedFloat;

// Why
mod engine;
use engine::systems::sand_movement::*;
use crate::engine::plugins::instancing::{CustomMaterialPlugin, InstanceData, InstanceMaterialData};
// use engine::systems::water_movement::*;
use crate::engine::systems::update_pixel_color::update_pixel_color;

#[derive(Clone, Copy, Ord, PartialOrd, Eq, PartialEq)]
struct Vect3 {
    x: OrderedFloat<f32>,
    y: OrderedFloat<f32>,
    z: OrderedFloat<f32>,
}
impl Vect3 {
    pub fn new(x: f32, y: f32, z: f32) -> Vect3 {
        Vect3 {
            x: OrderedFloat::from(x),
            y: OrderedFloat::from(y),
            z: OrderedFloat::from(z)
        }
    }
    pub fn from_vec3(vec3: Vec3) -> Vect3 {
        Vect3::new(vec3.x, vec3.y, vec3.z)
    }

    // pub fn clone(&self) -> Vect3 {
    //     Vect3::new(*self.x, *self.y, *self.z)
    // }

    pub fn to_vec3(&self) -> Vec3 {
        Vec3::new(*self.x, *self.y, *self.z)
    }

    // pub fn to_index(&self) -> u128 {
    //     let a = self.x as u128;
    //     let b = self.y as u128;
    //     let c = self.z as u128;
    //     let mut res = 0u128;
    //     res = res << 32 | a;
    //     res = res << 32 | b;
    //     res = res << 32 | c;
    //     // let wtf = c << 64 | (b << 32 | a);
    //     // println!("RES {res}");
    //     return res;
    // }
    //
    // pub fn from_index(index: u128) -> Vect3 {
    //     let c = index & 0x000000FFu128;
    //     let b = (index >> 32) & 0x000000FFu128;
    //     let a = index >> 64;
    //     return Vect3::new(a as f32, b as f32, c as f32);
    // }

    // private static ulong Combine(int a, int b) {
    // uint ua = (uint)a;
    // ulong ub = (uint)b;
    // return ub <<32 | ua;
    // }
    // private static void Decombine(ulong c, out int a, out int b) {
    // a = (int)(c & 0xFFFFFFFFUL);
    // b = (int)(c >> 32);
    // }
}

impl ops::Add<Vect3> for Vect3 {
    type Output = Vect3;

    fn add(self, _rhs: Vect3) -> Vect3 {
        Vect3::new(
            *self.x + *_rhs.x,
            *self.y + *_rhs.y,
            *self.z + *_rhs.z
        )
    }
}

impl ops::Add<Vec3> for Vect3 {
    type Output = Vect3;

    fn add(self, _rhs: Vec3) -> Vect3 {
        Vect3::new(
            *self.x + _rhs.x,
            *self.y + _rhs.y,
            *self.z + _rhs.z
        )
    }
}

impl ops::AddAssign<Vec3> for Vect3 {
    fn add_assign(&mut self, rhs: Vec3) {
        self.x += rhs.x;
        self.y += rhs.y;
        self.z += rhs.z;
    }
}

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
        .add_plugins(DefaultPlugins.set(ImagePlugin::default_nearest()))
        .add_plugins(PanOrbitCameraPlugin)
        .add_plugins(DefaultPickingPlugins)
        .add_plugins(CustomMaterialPlugin)
        .insert_resource(DebugPickingMode::Normal)

        .init_resource::<PixelPositions>()

        .insert_resource(Time::<Fixed>::from_seconds(0.7))

        .add_systems(Startup, (setup, create_pixels).chain())

        // .add_systems(FixedPreUpdate, update_transforms_list)
        // .add_systems(FixedUpdate, collision_system)
        // .add_systems(FixedUpdate, fix_y)
        // .add_systems(FixedUpdate, sideways_movement)

        .add_systems(FixedUpdate, sand_movement)
        // .add_systems(FixedUpdate, water_movement)

        // .add_systems(FixedPostUpdate, update_pixel_color)
        // .add_systems(PreUpdate, update_render_pixels)
        // .add_systems(Render, my_render)
        // .add_systems(Update, check_destroy)

        .run();
}

fn update_render_pixels(
    mut pixels: ResMut<PixelPositions>,
    mut materials: ResMut<Assets<StandardMaterial>>,
    // render_pixel_query: Query<Entity, With<RenderPixel>>,
    mut render_transform_query: Query<(&Transform, &Handle<StandardMaterial>), With<RenderPixel>>,
) {
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

    for (transform, material) in render_transform_query.iter() {
    // render_transform_query
    //     .par_iter_mut()
    //     .batching_strategy(BatchingStrategy::fixed(32))
    //     .for_each(|(transform, material)| {
            // if let Ok((transform, material)) = render_transform_query.get_mut(entity) {
            let key = Vect3::from_vec3(transform.translation);
            let mut color = Color::NONE;

            // If there's a pixel at the position
            if let Some(map_value) = pixels.map.get(&key) {
                color = match &map_value.pixel_type {
                    PixelType::Invalid => Color::GREEN, // HOW
                    PixelType::Sand => Color::ORANGE,
                    PixelType::Water => {
                        let mut c = Color::BLUE;
                        c.set_a(0.4);
                        c
                    },
                    _ => Color::NONE,
                };
                // materials.get_mut(material).unwrap().base_color = color;
            }

            // pix.pixel_type = map_value.clone().pixel_type;
            // No exist, change color to clear
            // pix.pixel_type = PixelType::Invalid;
            materials.get_mut(material).unwrap().base_color = color;
            // pixels.is_colors_dirty = true;

            // transform.rotate_z(-PI / 2. * time.delta_seconds());
            // if (transform.translation.y < 0.0) {
            // if pixel.destroy {
            //     commands.entity(parent).despawn();
            // }
        // }
    }

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

    let shape = meshes.add(Cuboid {
        half_size: Vec3 {
            x: 0.5,
            y: 0.5,
            z: 0.5,
        }
    });

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

    commands.spawn((
        meshes.add(Cuboid::new(0.5, 0.5, 0.5)),
        SpatialBundle::INHERITED_IDENTITY,
        InstanceMaterialData(
            (1..=10)
                .flat_map(|x| (1..=10).map(move |y| (x as f32 / 10.0, y as f32 / 10.0)))
                .map(|(x, y)| InstanceData {
                    position: Vec3::new(x * 10.0 - 5.0, y * 10.0 - 5.0, 0.0),
                    scale: 1.0,
                    color: Color::hsla(x * 360., y, 0.5, 1.0).as_rgba_f32(),
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

fn create_pixels(
    mut pixels: ResMut<PixelPositions>
) {
    for a in 0..=50 {
        for b in 0..=50 {
            for c in 0..=50 {
                if b % 3 == 0 {
                // let entity = spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32, PixelType::Sand);
                // commands.entity(entity);
                    pixels.map.insert(Vect3::new(a as f32, b as f32, c as f32)/*.to_index()*/, Pixel {
                        pixel_type: PixelType::Sand,
                        dont_move: false,
                        pixel_temperature: 0.0,
                    });
                } else if b % 2 == 0 {
                    pixels.map.insert(Vect3::new(a as f32, b as f32, c as f32)/*.to_index()*/, Pixel {
                        pixel_type: PixelType::Water,
                        dont_move: false,
                        pixel_temperature: 0.0,
                    });
                } else {
                    pixels.map.insert(Vect3::new(a as f32, b as f32, c as f32)/*.to_index()*/, Pixel {
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