use std::cmp::Ordering;
use std::collections::BTreeMap;
use std::f32::consts::PI;
// use rand::prelude::*;
use bevy::prelude::*;
use bevy_mod_picking::prelude::*;
use bevy_panorbit_camera::{PanOrbitCamera, PanOrbitCameraPlugin};

// Why
mod engine;
use engine::systems::sand_movement::*;
use engine::systems::water_movement::*;
use crate::engine::systems::update_pixel_color::update_pixel_color;

struct Vect3 {
    x: f32,
    y: f32,
    z: f32,
}
impl Vect3 {
    pub fn new(x: f32, y: f32, z: f32) -> Vect3 {
        Vect3 { x: x, y: y, z: z }
    }
    pub fn from_vec3(vec3: Vec3) -> Vect3 {
        Vect3::new(vec3.x, vec3.y, vec3.z)
    }

    pub fn to_index(&self) -> u128 {
        let a = self.x as u128;
        let b = self.y as u128;
        let c = self.z as u128;
        let mut res = 0u128;
        res = res << 32 | a;
        res = res << 32 | b;
        res = res << 32 | c;
        // let wtf = c << 64 | (b << 32 | a);
        // println!("RES {res}");
        return res;
    }

    pub fn from_index(index: u128) -> Vect3 {
        let c = index & 0x000000FFu128;
        let b = (index >> 32) & 0x000000FFu128;
        let a = index >> 64;
        return Vect3::new(a as f32, b as f32, c as f32);
    }

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

enum PixelType {
    Unmovable = -2,
    Invalid = -1,
    Sand,
    Water,
    Lava,
    Steam,
    Rock,
}

struct Pixel {
    pixel_type: PixelType,
    pixel_temperature: f32,
    dont_move: bool,
}


#[derive(Resource, Default)]
struct PixelPositions {
    // positions: Vec<PixelTransform>,
    map: BTreeMap<u128, Pixel>,
}

fn main() {
    App::new()
        .add_plugins(DefaultPlugins.set(ImagePlugin::default_nearest()))
        .add_plugins(PanOrbitCameraPlugin)
        .add_plugins(DefaultPickingPlugins)
        .insert_resource(DebugPickingMode::Normal)

        .init_resource::<PixelPositions>()

        .insert_resource(Time::<Fixed>::from_seconds(0.08))

        .add_systems(Startup, setup)

        .add_systems(FixedPreUpdate, update_transforms_list)
        // .add_systems(FixedUpdate, collision_system)
        // .add_systems(FixedUpdate, fix_y)
        // .add_systems(FixedUpdate, sideways_movement)

        .add_systems(FixedUpdate, sand_movement)
        .add_systems(FixedUpdate, water_movement)

        .add_systems(FixedPostUpdate, update_pixel_color)
        .add_systems(Update, check_destroy)

        .run();
}


fn update_transforms_list(
    mut pixel_transforms: ResMut<PixelPositions>,
    pixels: Query<(&Transform, &Pixel)>
) {
    // pixel_transforms.positions.clear();
    pixel_transforms.map.clear();
    for (tr, pix) in pixels.iter() {
        pixel_transforms.map.insert(Vect3::from_vec3(tr.translation).to_index(), Pixel {
            pixel_temperature: 0.0,
            pixel_type: PixelType::Water,
        });
        // pixel_transforms.positions.push(PixelTransform {
        //     transform: tr.clone(),
        //     entity: pix.id,
        // });
    }

    // for (k, v) in pixel_transforms.map.iter() {
    //     // let v_t = &v.pixel_type;
    //     let v_tr = &v.transform;
    //     let c = k & 0x000000FFu128;
    //     let b = (k >> 32) & 0x000000FFu128;
    //     let a = k >> 64;
    //     // a = (int)(c & 0xFFFFFFFFUL);
    //     // b = (int)(c >> 32);
    //     println!("Map: [{a} {b} {c}] : {v_tr}");
    // }
    return;
}

fn check_destroy(
    mut commands: Commands,
    parents_query: Query<Entity, With<Pixel>>,
    mut pixel_query: Query<&mut Pixel>,
) {
    // For each entity with the Pixel component
    for parent in &parents_query {
        // Pull out the Pixel component
        if let Ok(mut pixel) = pixel_query.get_mut(parent) {
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
    id: Entity,
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
) -> Entity {
    // let mut rng = rand::thread_rng();

    let entity = commands
        .spawn(PbrBundle {
            mesh: shape,
            material: materials.add(Color::NONE),
            transform: Transform::from_xyz(
                x,//rng.gen_range(-50..=50) as f32,
                y,//10.0,
                z//rng.gen_range(-50..=50) as f32,
            ),
            ..default()
        })
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
        .insert(PickableBundle::default()).id();

    commands.entity(entity).insert(RenderPixel {
        dont_move: false,
        pixel_type: pixel_type,
        destroy: false,
        id: entity,
    });

    return entity;
}

fn setup(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
    mut pixels: ResMut<PixelPositions>
) {
    commands.insert_resource(PixelPositions {
        map: BTreeMap::new(),
    });

    let shapes = [
        meshes.add(Cuboid {
            half_size: Vec3 {
                x: 0.5,
                y: 0.5,
                z: 0.5,
            }
        }),
    ];

    for shape in shapes.into_iter() {
        for a in 0..=10 {
            for b in 20..=25 {
                for c in 0..=10 {
                    if b % 2 == 0 {
                        // let entity = spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32, PixelType::Sand);
                        // commands.entity(entity);
                        pixels.map.insert(Vect3::new(a as f32, b as f32, c as f32).to_index(), Pixel {
                            pixel_type: PixelType::Sand,
                            pixel_temperature: 0.0,
                        });
                    } else {
                        // let entity = spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32, PixelType::Water);
                        // commands.entity(entity);
                        pixels.map.insert(Vect3::new(a as f32, b as f32, c as f32).to_index(), Pixel {
                            pixel_type: PixelType::Water,
                            pixel_temperature: 0.0,
                        });
                    }
                }
            }
        }
    }

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
