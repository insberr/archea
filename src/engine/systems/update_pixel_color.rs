use bevy::asset::Assets;
use bevy::pbr::{StandardMaterial};
use bevy::prelude::{Color, Handle, ResMut};
use bevy::prelude::Query;
use crate::{Pixel, PixelType};

pub fn update_pixel_color(
    mut materials: ResMut<Assets<StandardMaterial>>,
    mut pixels: Query<(&Pixel, &Handle<StandardMaterial>)>
) {
    for (pix, mat) in pixels.iter_mut() {
        let pix_type = &pix.pixel_type;
        let color = match pix_type {
            PixelType::Sand => {
                let mut c = Color::RED;
                c.set_a(0.7);
                c
            },
            PixelType::Water => {
                let mut c = Color::YELLOW;
                c.set_a(0.7);
                c
            },
            _ => Color::YELLOW,
        };

        let material = materials.get_mut(mat).unwrap();
        material.base_color = color;
    }
}