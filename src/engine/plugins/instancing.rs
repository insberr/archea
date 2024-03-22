/*
 * File copied from https://github.com/DavidHospital/FrostyGrass/blob/main/src/render/instancing.rs
 * Credit to their grass library
 *
 * This has been slightly modified by me to work with bevy 0.13., I may send a pr updating
 *   in the grass library too.
 * Modifications include:
 * - Update to bevy 0.13.
 * - Adding a color value (for my needs)
 * - Adding comments that explain the code a little better (Soon)
 */

use bevy::utils::nonmax::NonMaxU32;
use std::marker::PhantomData;

use bevy::core_pipeline::core_3d::Transparent3d;
use bevy::ecs::query::QueryItem;
use bevy::ecs::system::lifetimeless::{Read, SRes};
use bevy::ecs::system::SystemParamItem;
use bevy::pbr::{
    MeshPipeline, MeshPipelineKey, RenderMeshInstances, SetMaterialBindGroup, SetMeshBindGroup,
    SetMeshViewBindGroup,
};
use bevy::prelude::*;
use bevy::render::extract_component::{ExtractComponent, ExtractComponentPlugin};
use bevy::render::mesh::{GpuBufferInfo, MeshVertexBufferLayout};
use bevy::render::render_asset::RenderAssets;
use bevy::render::render_phase::{
    AddRenderCommand, DrawFunctions, PhaseItem, RenderCommand, RenderCommandResult, RenderPhase,
    SetItemPipeline, TrackedRenderPass,
};
use bevy::render::render_resource::{
    BindGroupLayout, Buffer, BufferInitDescriptor, BufferUsages, PipelineCache,
    RenderPipelineDescriptor, SpecializedMeshPipeline, SpecializedMeshPipelineError,
    SpecializedMeshPipelines, VertexAttribute, VertexBufferLayout, VertexFormat, VertexStepMode,
};
use bevy::render::renderer::RenderDevice;
use bevy::render::view::ExtractedView;
use bevy::render::{Render, RenderApp, RenderSet};
use bytemuck::Pod;

pub trait InstancedMaterial: Send + Sync + Clone + Pod
    where
        Self::M: Material,
{
    type M;

    fn shader_path() -> &'static str;

    fn material_bind_group_layout<M: Material>(render_device: &RenderDevice) -> BindGroupLayout {
        M::bind_group_layout(render_device)
    }
}

#[derive(Component, Clone)]
pub struct InstanceData<D> {
    pub data: Vec<D>,
    pub mesh: Handle<Mesh>,
}

impl<D: InstancedMaterial> ExtractComponent for InstanceData<D> {
    type QueryData = &'static InstanceData<D>;
    type QueryFilter = ();
    type Out = Self;

    fn extract_component(item: QueryItem<'_, Self::QueryData>) -> Option<Self> {
        Some(item.clone())
    }
}

pub struct InstancingPlugin<D>(PhantomData<D>);

impl<D: 'static> Plugin for InstancingPlugin<D>
    where
        D: InstancedMaterial,
{
    fn build(&self, app: &mut App) {
        app.add_plugins((ExtractComponentPlugin::<InstanceData<D>>::default(),));
        app.sub_app_mut(RenderApp)
            .add_render_command::<Transparent3d, DrawInstanced<D::M>>()
            .init_resource::<SpecializedMeshPipelines<InstancingPipeline<D>>>()
            .add_systems(
                Render,
                (
                    queue_custom::<D>.in_set(RenderSet::QueueMeshes),
                    (prepare_instance_buffers::<D>).in_set(RenderSet::PrepareResources),
                ),
            );
    }

    fn finish(&self, app: &mut App) {
        app.sub_app_mut(RenderApp)
            .init_resource::<InstancingPipeline<D>>();
    }
}

impl<D> Default for InstancingPlugin<D> {
    fn default() -> Self {
        Self(PhantomData)
    }
}

type DrawInstanced<M> = (
    SetItemPipeline,
    SetMeshViewBindGroup<0>,
    SetMeshBindGroup<1>,
    SetMaterialBindGroup<M, 2>,
    DrawMeshInstanced,
);

struct DrawMeshInstanced;

impl<P: PhaseItem> RenderCommand<P> for DrawMeshInstanced {
    type Param = (SRes<RenderAssets<Mesh>>, SRes<RenderMeshInstances>);
    // type ViewWorldQuery = ();
    type ViewQuery = ();
    type ItemQuery = Read<InstanceBuffer>;
    // type ItemWorldQuery = Read<InstanceBuffer>;

    #[inline]
    fn render<'w>(
        item: &P,
        _view: (),
        instance_buffer: Option<&'w InstanceBuffer>,
        (meshes, render_mesh_instances): SystemParamItem<'w, '_, Self::Param>,
        pass: &mut TrackedRenderPass<'w>,
    ) -> RenderCommandResult {
        let Some(mesh_instance) = render_mesh_instances.get(&item.entity()) else {
            return RenderCommandResult::Failure;
        };
        let gpu_mesh = match meshes.into_inner().get(mesh_instance.mesh_asset_id) {
            Some(gpu_mesh) => gpu_mesh,
            None => return RenderCommandResult::Failure,
        };

        pass.set_vertex_buffer(0, gpu_mesh.vertex_buffer.slice(..));
        pass.set_vertex_buffer(1, instance_buffer.unwrap().buffer.slice(..));

        match &gpu_mesh.buffer_info {
            GpuBufferInfo::Indexed {
                buffer,
                index_format,
                count,
            } => {
                pass.set_index_buffer(buffer.slice(..), 0, *index_format);
                pass.draw_indexed(0..*count, 0, 0..instance_buffer.unwrap().length as u32);
            }
            GpuBufferInfo::NonIndexed => {
                pass.draw(0..gpu_mesh.vertex_count, 0..instance_buffer.unwrap().length as u32);
            }
        }
        RenderCommandResult::Success
    }
}

#[derive(Resource)]
struct InstancingPipeline<D> {
    mesh_pipeline: MeshPipeline,
    material_layout: BindGroupLayout,
    shader: Handle<Shader>,
    marker: PhantomData<D>,
}

impl<D> SpecializedMeshPipeline for InstancingPipeline<D> {
    type Key = MeshPipelineKey;

    fn specialize(
        &self,
        key: Self::Key,
        layout: &MeshVertexBufferLayout,
    ) -> Result<RenderPipelineDescriptor, SpecializedMeshPipelineError> {
        let mut descriptor = self.mesh_pipeline.specialize(key, layout)?;

        descriptor.vertex.shader_defs.push("VERTEX_COLORS".into());
        descriptor.vertex.shader = self.shader.clone();
        descriptor.vertex.buffers.push(VertexBufferLayout {
            array_stride: std::mem::size_of::<D>() as u64,
            step_mode: VertexStepMode::Instance,
            attributes: vec![
                VertexAttribute {
                    format: VertexFormat::Float32x4,
                    offset: 0,
                    shader_location: 3, // shader locations 0-2 are taken up by Position, Normal and UV attributes
                },
                // Color
                VertexAttribute {
                    format: VertexFormat::Float32x4,
                    offset: VertexFormat::Float32x4.size(),
                    shader_location: 4,
                },
            ],
        });
        descriptor.fragment.as_mut().unwrap().shader = self.shader.clone();
        descriptor.fragment.as_mut().unwrap().shader_defs = descriptor.vertex.shader_defs.clone();

        // insberr: AHA it was this. the 1 needs to be a 2, and the
        //     SetMeshBindGroup<1>,
        //     SetMaterialBindGroup<M, 2>,
        // need to be swapped (above is correct)
        descriptor.layout.insert(2, self.material_layout.clone());
        Ok(descriptor)
    }
}

impl<D: InstancedMaterial> FromWorld for InstancingPipeline<D> {
    fn from_world(world: &mut World) -> Self {
        let asset_server = world.resource::<AssetServer>();
        let render_device = world.resource::<RenderDevice>();

        let shader = asset_server.load(D::shader_path());
        let mesh_pipeline = world.resource::<MeshPipeline>();

        Self {
            shader,
            mesh_pipeline: mesh_pipeline.clone(),
            material_layout: D::material_bind_group_layout::<D::M>(render_device),
            marker: PhantomData,
        }
    }
}

#[allow(clippy::too_many_arguments)]
fn queue_custom<D: 'static>(
    transparent_3d_draw_functions: Res<DrawFunctions<Transparent3d>>,
    custom_pipeline: Res<InstancingPipeline<D>>,
    msaa: Res<Msaa>,
    mut pipelines: ResMut<SpecializedMeshPipelines<InstancingPipeline<D>>>,
    pipeline_cache: Res<PipelineCache>,
    meshes: Res<RenderAssets<Mesh>>,
    render_mesh_instances: Res<RenderMeshInstances>,
    material_meshes: Query<(Entity, &InstanceData<D>)>,
    mut views: Query<(&ExtractedView, &mut RenderPhase<Transparent3d>)>,
) where
    D: InstancedMaterial,
{
    let draw_custom = transparent_3d_draw_functions
        .read()
        .id::<DrawInstanced<D::M>>();

    let msaa_key = MeshPipelineKey::from_msaa_samples(msaa.samples());

    for (view, mut transparent_phase) in &mut views {
        let view_key = msaa_key | MeshPipelineKey::from_hdr(view.hdr);
        let rangefinder = view.rangefinder3d();
        for (entity, instance_data) in &material_meshes {
            let Some(mesh_instance) = render_mesh_instances.get(&entity) else {
                continue;
            };
            let Some(mesh) = meshes.get(instance_data.mesh.id()) else {
                continue;
            };
            let key = view_key | MeshPipelineKey::from_primitive_topology(mesh.primitive_topology);
            let pipeline = pipelines
                .specialize(&pipeline_cache, &custom_pipeline, key, &mesh.layout)
                .unwrap();
            transparent_phase.add(Transparent3d {
                entity,
                pipeline,
                draw_function: draw_custom,
                distance: rangefinder
                    .distance_translation(&mesh_instance.transforms.transform.translation),
                // distance: 0.,
                batch_range: 0..1,
                dynamic_offset: None,
            });
        }
    }
}

#[derive(Component)]
struct InstanceBuffer {
    buffer: Buffer,
    length: usize,
}

fn prepare_instance_buffers<D: 'static>(
    mut commands: Commands,
    query: Query<(Entity, &InstanceData<D>)>,
    render_device: Res<RenderDevice>,
) where
    D: InstancedMaterial,
{
    for (index, (entity, instance_data)) in query.iter().enumerate() {
        let buffer = render_device.create_buffer_with_data(&BufferInitDescriptor {
            label: Some(&*("instance data buffer".to_owned() + &*index.to_string())),
            contents: bytemuck::cast_slice(instance_data.data.as_slice()),
            usage: BufferUsages::VERTEX | BufferUsages::COPY_DST,
        });
        commands.entity(entity).insert(InstanceBuffer {
            buffer,
            length: instance_data.data.len(),
        });
    }
}
