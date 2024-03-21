#import bevy_pbr::mesh_functions::{get_model_matrix, mesh_position_local_to_clip}
//#import bevy_pbr::mesh_view_bindings::globals
#import bevy_pbr::forward_io::{VertexOutput,FragmentOutput}
#import bevy_pbr::pbr_fragment::pbr_input_from_standard_material
#import bevy_pbr::pbr_functions::{apply_pbr_lighting, main_pass_post_lighting_processing,alpha_discard}
#import bevy_pbr::pbr_bindings::material


struct Vertex {
    @location(0) position: vec3<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) uv: vec2<f32>,

    @location(3) i_pos_scale: vec4<f32>,
    // The custom color overide for now. I think later it might be better to just make seperate instances
    //  for each color so eahc can actually use the material LOL.
    @location(4) i_pos_color: vec4<f32>,
};

@vertex
fn vertex(vertex: Vertex) -> VertexOutput {
    var position = vertex.position * vertex.i_pos_scale.w + vertex.i_pos_scale.xyz;

    var out: VertexOutput;
    out.position = mesh_position_local_to_clip(
        get_model_matrix(0u),
        vec4<f32>(position, 1.0)
    );

	out.world_position = vec4<f32>(position, 1.);
	out.world_normal = vertex.normal;
    out.uv = vertex.uv;

    out.color = vertex.i_pos_color;

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

    pbr_input.material.base_color = in.color;

    // pbr_input.material.base_color = alpha_discard(pbr_input.material, pbr_input.material.base_color);

    var out: FragmentOutput;
    if (is_front) {
    out.color = apply_pbr_lighting(pbr_input);
    out.color = main_pass_post_lighting_processing(pbr_input, out.color);
    } else {
    out.color = vec4(0.0,0.0,0.0,0.0);
    }

    return out;
}
