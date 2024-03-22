use std::fmt::Formatter;
use std::ops;
use std::ops::Mul;
use bevy::math::Vec3;
use ordered_float::OrderedFloat;

#[derive(Clone, Copy, Ord, PartialOrd, Eq, PartialEq)]
pub struct Vect3i {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}

impl Vect3i {
    pub fn new(x: i32, y: i32, z: i32) -> Vect3i {
        Vect3i { x, y, z }
    }
}

impl ops::Add<Vect3i> for Vect3i {
    type Output = Vect3i;

    fn add(self, _rhs: Vect3i) -> Vect3i {
        Vect3i::new(
            self.x + _rhs.x,
            self.y + _rhs.y,
            self.z + _rhs.z
        )
    }
}

impl Mul for Vect3i {
    type Output = Vect3i;

    fn mul(self, rhs: Self) -> Self::Output {
        Vect3i::new(
            self.x * rhs.x,
            self.y * rhs.y,
            self.z * rhs.z
        )
    }
}

impl std::fmt::Display for Vect3i {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "({}, {}, {})", self.x, self.y, self.z)
    }
}

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
}

impl std::fmt::Display for Vect3 {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "({}, {}, {})f", self.x, self.y, self.z)
    }
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

impl ops::Add<&Vec3> for &Vect3 {
    type Output = Vect3;

    fn add(self, _rhs: &Vec3) -> Vect3 {
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
