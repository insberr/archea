use std::collections::BTreeMap;
use std::fmt::Formatter;
use ordered_float::FloatCore;
use crate::engine::stuff::vect3::{Vect3, Vect3i};

#[derive(Clone)]
pub struct Chunks {
    // Current number of chunks in the x, y, and z directions
    pub number_of_chunks: Vect3i,

    // Max number of chunks allowed in the x, y, and z directions
    pub max_number_of_chunks: Vect3i,

    // Default chunk size
    pub default_chunk_bounds: Vect3i,

    // The chunks
    pub chunks: BTreeMap<Vect3i, Chunk>,
}

impl Chunks {
    pub fn new(max: Vect3i, bounds: Vect3i) -> Chunks {
        Chunks {
            number_of_chunks: Vect3i::new(0, 0, 0),
            max_number_of_chunks: max,
            default_chunk_bounds: bounds,
            chunks: BTreeMap::new()
        }
    }

    pub fn create_chunk(&mut self, chunk_index: Vect3i) {
        if self.chunks.contains_key(&chunk_index) {
            return panic!("Chunk: '{chunk_index}' already exists, dumbass");
        }

        self.chunks.insert(chunk_index, Chunk::new(self.default_chunk_bounds));
    }

    pub fn get_chunk_or_create(&mut self, chunk_index: Vect3i) -> &mut Chunk {
        if self.chunks.contains_key(&chunk_index) {
            self.chunks.get_mut(&chunk_index).unwrap()
        } else {
            self.create_chunk(chunk_index);
            // This could possibly create an infinite loop...
            //   but that's if something goes really wrong...
            self.get_chunk_or_create(chunk_index)
        }
    }

    pub fn create_pixel(&mut self, position: Vect3, pixel_type: PixelType) {
        // Figure out what chunk this goes in
        // Given the default bounds, we can do some math, I think.
        let the_chunk_we_are_in = Vect3i::new(
            ((*position.x + (self.default_chunk_bounds.x as f32 * 0.5 * (if position.x.is_sign_positive() {1.0} else {-1.0}) )) / self.default_chunk_bounds.x as f32) as i32,
            ((*position.y + (self.default_chunk_bounds.y as f32 * 0.5 * (if position.y.is_sign_positive() {1.0} else {-1.0}) )) / self.default_chunk_bounds.y as f32) as i32,
             ((*position.z + (self.default_chunk_bounds.z as f32 * 0.5 * (if position.z.is_sign_positive() {1.0} else {-1.0}) )) / self.default_chunk_bounds.z as f32) as i32
        );

        let mut chunk = self.get_chunk_or_create(the_chunk_we_are_in);
        chunk.set_pixel(&position, Pixel::new(pixel_type));
    }

    pub fn print_chunks(&self) {
        for (chunk_pos, chunk) in self.chunks.iter() {
            println!("Chunk {chunk_pos}:\n\tPixels: {0}", chunk.pixels.len());
            // for (pixel_pos, pixel) in chunk.pixels.iter() {
            //     println!("\tPixel Type {0} at {pixel_pos}", pixel.pixel_type);
            // }
        }
    }

    pub fn update_chunks(&mut self) {
        // Figure out how to do this ...
    }
}

#[derive(Clone)]
pub struct Chunk {
    // The size of the chunk
    pub bounds: Vect3i,

    // The pixels in this chunk
    pub pixels: BTreeMap<Vect3, Pixel>,
}

impl Chunk {
    pub fn new(bounds: Vect3i) -> Chunk {
        Chunk {
            bounds: bounds,
            pixels: BTreeMap::new(),
        }
    }

    pub fn set_pixel(&mut self, &position: &Vect3, pixel: Pixel) {
        self.pixels.insert(position, pixel);
    }

    pub fn update_pixels(&mut self) {
        //
    }
}

#[derive(PartialEq, Clone)]
pub enum PixelType {
    Invalid = -1,
    Unmovable = 0,
    Sand,
    Water,
    Lava,
    Steam,
    Rock,
}

impl std::fmt::Display for PixelType {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let value = match self {
            PixelType::Invalid => "Invalid",
            PixelType::Unmovable => "Unmoveable",
            PixelType::Sand=> "Sand",
            PixelType::Water=> "Water",
            PixelType::Lava=> "Lava",
            PixelType::Steam=> "Steam",
            PixelType::Rock=> "Rock",
            _ => "Actually Invalid (HOW ???)",
        };
        write!(f, "{value}")
    }
}

#[derive(Clone)]
pub struct Pixel {
    // General pixel info
    pub pixel_type: PixelType, // Temporary
    pub dont_move: bool,

    // State
    pub temperature: f32,

    // Properties
    pub weight: f32,
}

impl Pixel {
    pub fn new(pixel_type: PixelType) -> Pixel {
        Pixel {
            pixel_type,
            dont_move: false,
            temperature: 0.0,
            weight: 0.0,
        }
    }
}