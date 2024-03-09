use bevy::asset::Assets;
use bevy::pbr::{StandardMaterial};
use bevy::prelude::{Color, Handle, ResMut};
use bevy::prelude::Query;
use crate::{PixelType, RenderPixel};

pub fn update_pixel_color(
    mut materials: ResMut<Assets<StandardMaterial>>,
    mut pixels: Query<(&RenderPixel, &Handle<StandardMaterial>)>
) {
    for (pix, mat) in pixels.iter_mut() {
        let pix_type = &pix.pixel_type;
        let color = match pix_type {
            PixelType::Sand => {
                let mut c = Color::ORANGE;
                c.set_a(1.0);
                c
            },
            PixelType::Water => {
                let mut c = Color::BLUE;
                c.set_a(0.4);
                c
            },
            _ => Color::YELLOW,
        };

        let material = materials.get_mut(mat).unwrap();
        material.base_color = color;
    }
}