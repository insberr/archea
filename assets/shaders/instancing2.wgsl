#import bevy_pbr::mesh_functions::{get_model_matrix, mesh_position_local_to_clip}
//#import bevy_pbr::mesh_view_bindings::globals
#import bevy_pbr::forward_io::{VertexOutput,FragmentOutput}
#import bevy_pbr::pbr_fragment::pbr_input_from_standard_material
#import bevy_pbr::pbr_functions::{apply_pbr_lighting, main_pass_post_lighting_processing,alpha_discard}
#import bevy_pbr::pbr_deferred_functions::deferred_output
#import bevy_pbr::pbr_bindings::material;

//@group(2) @binding(100)
//var mat_array_texture: texture_2d_array<f32>;
//@group(2) @binding(101)
//var mat_array_texture_sampler: sampler;

struct Vertex {
    @location(0) position: vec3<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) uv: vec2<f32>,

    @location(3) i_pos_scale: vec4<f32>,
    @location(4) i_pos_color: vec4<f32>,
};

fn hash(in: f32) -> f32 {
	let large = i32(in * 371311.);
	return f32(large % 91033) / 91033.;
}

fn rotation_axis_matrix(axis: vec3<f32>, angle: f32) -> mat3x3<f32> {
	let x = axis.x;
	let y = axis.y;
	let z = axis.z;
	let sin_a = sin(angle);
	let cos_a = cos(angle);
	let cos_a_i = 1. - cos_a;
	return mat3x3<f32>(
		cos_a + x*x * cos_a_i, x*y * cos_a_i - z * sin_a, x*z * cos_a_i + y * sin_a,
		y*x * cos_a_i + z * sin_a, cos_a + y*y * cos_a_i, y*z * cos_a_i - x * sin_a,
		z*x * cos_a_i - y * sin_a, z*y * cos_a_i + x * sin_a, cos_a + z*z * cos_a_i,
	);
}

fn rotate_axis(point: vec3<f32>, axis: vec3<f32>, angle: f32) -> vec3<f32> {
	return rotation_axis_matrix(axis, angle) * point;
}

@vertex
fn vertex(vertex: Vertex) -> VertexOutput {
//	let hashed = hash(vertex.i_pos_scale.x + vertex.i_pos_scale.z);
    var position = vertex.position * vertex.i_pos_scale.w + vertex.i_pos_scale.xyz;
//    let lambda = clamp(position.y - vertex.i_pos_scale.y, 0., 1.);
//	position.x += sin(1. * 1. + position.x * 0.01 + position.z * 0.01) * 0.3 * lambda + sin(1. * hash(position.x + position.z)) * 0.3 * lambda;

    var out: VertexOutput;
    out.position = mesh_position_local_to_clip(
        get_model_matrix(0u),
        vec4<f32>(position, 1.0)
    );
    out.uv = vertex.uv;
	out.world_position = vec4<f32>(position, 1.);
	out.world_normal = vertex.normal;
	let color = vertex.i_pos_color;
//	out.color = mix(0.1 * color, 1.2 * color, lambda);
out.color = color;
    return out;
}

@fragment
fn fragment(
	in: VertexOutput,
    @builtin(front_facing) is_front: bool,
) -> FragmentOutput {
//	let pbr_input = pbr_input_from_standard_material(in, is_front);
//    var color = apply_pbr_lighting(pbr_input);
//    color = main_pass_post_lighting_processing(pbr_input, color);
//	return color;
//var out: FragmentOutput;

var standard_in: VertexOutput;
    standard_in.position = in.position;
    standard_in.world_normal = in.world_normal;
    standard_in.world_position = in.world_position;
    standard_in.uv = in.uv;
    standard_in.color = in.color;
    standard_in.instance_index = in.instance_index;
    var pbr_input = pbr_input_from_standard_material(standard_in, is_front);

        var tex_face = 0;

        // determine texture index based on normal
        if in.world_normal.y == 0.0 {
            tex_face = 1;
        } else if in.world_normal.y < 0.0 {
            tex_face = 2;
        }

//        pbr_input.material.base_color = textureSample(mat_array_texture, mat_array_texture_sampler, in.uv, 0u);
        pbr_input.material.base_color = pbr_input.material.base_color * in.color;

//        pbr_input.material.base_color = alpha_discard(pbr_input.material, pbr_input.material.base_color);
#ifdef PREPASS_PIPELINE
    let out = deferred_output(in, pbr_input);
#else
    var out: FragmentOutput;
    out.color = apply_pbr_lighting(pbr_input);
    out.color = main_pass_post_lighting_processing(pbr_input, out.color);
#endif


    return out;
}
