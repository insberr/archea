#import bevy_pbr::forward_io::{VertexOutput, FragmentOutput};

@fragment
fn fragment(
    in: VertexOutput,
    @location(8) tex_idx: vec3<u32>,
) -> FragmentOutput {
    var out: FragmentOutput;

    // tex_idx will be the array produced by the texture_index_mapper
    // You can use it to send three unsigned integers to the shader
    // based on the voxel type.

    // default color
    out.color = vec4<f32>(1.0, 0.0, 1.0, 1.0);

    // Invalid
    if (tex_idx[0] == 0u) {
        out.color = vec4<f32>(1.0, 0.0, 1.0, 1.0);
    }
    // Sand
    if (tex_idx[0] == 1u) {
        out.color = vec4<f32>(1.0, 0.65, 0.0, 1.0);
    }
    // Water
    if (tex_idx[0] == 2u) {
        out.color = vec4<f32>(0.0, 0.2, 0.9, 0.4);
    }
    // Lava
    if (tex_idx[0] == 3u) {
        out.color = vec4<f32>(1.0, 0.27, 0.0, 1.0);
    }
    // Steam
    if (tex_idx[0] == 4u) {
        out.color = vec4<f32>(1.0, 1.0, 1.0, 0.6);
    }
    // Rock
    if (tex_idx[0] == 5u) {
        out.color = vec4<f32>(0.5, 0.5, 0.5, 1.0);
    }

    // Multiply by the vertex color to get amient occlusion
    // out.color = out.color * in.color;

    return out;
}
