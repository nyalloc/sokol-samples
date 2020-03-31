//------------------------------------------------------------------------------
//  triangle-sapp.c
//  Simple 2D rendering from vertex buffer.
//------------------------------------------------------------------------------
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "dbgui/dbgui.h"
#include "triangle-sapp.glsl.h"

/* application state */
static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .gl = {
            .force_gles2 = sapp_gles2()
        },
        .mtl = {
            .device = sapp_metal_get_device(),
            .renderpass_descriptor_cb = sapp_metal_get_renderpass_descriptor,
            .drawable_cb = sapp_metal_get_drawable
        },
        .d3d11 = {
            .device = sapp_d3d11_get_device(),
            .device_context = sapp_d3d11_get_device_context(),
            .render_target_view_cb = sapp_d3d11_get_render_target_view,
            .depth_stencil_view_cb = sapp_d3d11_get_depth_stencil_view
        },
        .wgpu = {
            .device = sapp_wgpu_get_device(),
            .render_format = sapp_wgpu_get_render_format(),
            .render_view_cb = sapp_wgpu_get_render_view,
            .resolve_view_cb = sapp_wgpu_get_resolve_view,
            .depth_stencil_view_cb = sapp_wgpu_get_depth_stencil_view
        }
    });
    __dbgui_setup(1);

    /* a vertex buffer with 3 vertices */
    float vertices[] = {
        // positions            // colors
         0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
    };
    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .content = vertices,
        .label = "triangle-vertices"
    });

    /* create shader from code-generated sg_shader_desc */
    sg_shader shd = sg_make_shader(triangle_shader_desc());

    /* create a pipeline object (default render states are fine for triangle) */
    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        /* if the vertex layout doesn't have gaps, don't need to provide strides and offsets */
        .layout = {
            .attrs = {
                [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4
            }
        },
        .label = "triangle-pipeline"
    });

    /* a pass action to framebuffer to black */
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .action=SG_ACTION_CLEAR, .val={0.0f, 0.0f, 0.0f, 1.0f } }
    };
}

void frame(void) {
    sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 3, 1);
    __dbgui_draw();
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    __dbgui_shutdown();
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = __dbgui_event,
        .width = 640,
        .height = 480,
        .gl_force_gles2 = true,
        .window_title = "Triangle (sokol-app)",
    };
}
