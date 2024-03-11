use std::ops;
use bevy::math::Vec3;
use ordered_float::OrderedFloat;

#[derive(Clone, Copy, Ord, PartialOrd, Eq, PartialEq)]
pub struct Vect3 {
    pub x: OrderedFloat<f32>,
    pub y: OrderedFloat<f32>,
    pub z: OrderedFloat<f32>,
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
