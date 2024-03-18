use std::any::Any;
use bevy::asset::Assets;
use bevy::pbr::{StandardMaterial};
use bevy::prelude::{Color, Handle, ResMut};
use bevy::prelude::Query;
use crate::{PixelPositions, PixelType, RenderPixel};

pub fn update_pixel_color(
    // mut data: ResMut<PixelPositions>,
    mut materials: ResMut<Assets<StandardMaterial>>,
    mut pixels: Query<(&RenderPixel, &Handle<StandardMaterial>)>
) {
    // if data.is_colors_dirty == false {
    //     return;
    // }

    for (pix, mat) in pixels.iter_mut() {
        let color = match &pix.pixel_type {
            PixelType::Invalid => Color::NONE,
            PixelType::Sand => Color::ORANGE,
            PixelType::Water => {
                let mut c = Color::BLUE;
                c.set_a(0.4);
                c
            },
            _ => Color::NONE,
        };

        materials.get_mut(mat).unwrap().base_color = color;
    }

    // data.is_colors_dirty = false;
}