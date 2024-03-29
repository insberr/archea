#import bevy_pbr::mesh_functions::{get_model_matrix, mesh_position_local_to_clip}
#import bevy_pbr::{
    pbr_fragment::pbr_input_from_standard_material,
    pbr_functions::{apply_pbr_lighting, main_pass_post_lighting_processing},
}

struct Vertex {
    @location(0) position: vec3<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) uv: vec2<f32>,

    @location(3) i_pos_scale: vec4<f32>,
    @location(4) i_color: vec4<f32>,

    @location(5) temp: f32,
};

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) color: vec4<f32>,
};

@vertex
fn vertex(vertex: Vertex) -> VertexOutput {
    let position = vertex.position * vertex.i_pos_scale.w + vertex.i_pos_scale.xyz;
    var out: VertexOutput;
    // NOTE: Passing 0 as the instance_index to get_model_matrix() is a hack
    // for this example as the instance_index builtin would map to the wrong
    // index in the Mesh array. This index could be passed in via another
    // uniform instead but it's unnecessary for the example.
    out.clip_position = mesh_position_local_to_clip(
        get_model_matrix(1u),
        vec4<f32>(position, 1.0)
    );
    out.color = vertex.i_color;
    if (vertex.position.x < 0.01 && vertex.position.y < 0.01) {
        out.color = vec4(0.0, 0.0, 0.0, 1.0);
    }

//    if (vertex.temp < 100.0) {
//        let purple = vec4(1.0, 0.0, 1.0, 1.0);
//        let blue = vec4(0.0, 0.0, 1.0, 1.0);
//        let mixed = mix(purple, blue, vertex.temp / 100.0);
//        out.color = mixed;
//    } else if (vertex.temp < 200.0) {
//        let green = vec4(0.0, 1.0, 0.0, 1.0);
//        let blue = vec4(0.0, 0.0, 1.0, 1.0);
//        let mixed = mix(green, blue, vertex.temp / 200.0);
//        out.color = mixed;
//    }

    return out;
}

@fragment
fn fragment(in: VertexOutput) -> @location(0) vec4<f32> {
    var out: vec4<f32>;
    out = in.color;

    return out;
}