use bevy::app::{App, Plugin};
use bevy::core_pipeline::core_3d::Transparent3d;
use bevy::render::extract_component::ExtractComponentPlugin;
use bevy::render::render_resource::SpecializedMeshPipelines;
use bevy::render::{Render, RenderApp, RenderSet};
use crate::engine::plugins::instancing::InstanceMaterialData;

pub struct CubePositionUpdatePlugin;

impl Plugin for CubePositionUpdatePlugin {
    fn build(&self, app: &mut App) {
        app.add_plugins(ExtractComponentPlugin::<InstanceMaterialData>::default());
        app.sub_app_mut(RenderApp)
            .init_resource::<SpecializedMeshPipelines<crate::engine::plugins::instancing::CustomPipeline>>()
            .add_systems(
                Render,
                (
                    crate::engine::plugins::instancing::queue_custom.in_set(RenderSet::QueueMeshes),
                    crate::engine::plugins::instancing::prepare_instance_buffers.in_set(RenderSet::PrepareResources),
                ),
            );
    }

    fn finish(&self, app: &mut App) {
        app.sub_app_mut(RenderApp).init_resource::<crate::engine::plugins::instancing::CustomPipeline>();
    }
}