#pragma once

/**
 * @file config.hpp
 * @brief Configuration for Duck.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Should the editor be enabled? */
#define DK_EDITOR 1

/** Path to editor vertex shader. */
#define DK_EDITOR_VERTEX_SHADER ("shaders/editor-shader.vert.spv")

/** Path to editor fragment shader. */
#define DK_EDITOR_FRAGMENT_SHADER ("shaders/editor-shader.frag.spv")

/** Tell Duck Graphics whether to use debugging for Vulkan. */
#define DK_DEBUG_VULKAN 1

/** Interpolation rate for rigid bodies position. */
#define DK_PHYSICS_POSITION_INTERPOLATION_RATE (30.0f)

/** Interpolation rate for rigid bodies rotation. */
#define DK_PHYSICS_ROTATION_INTERPOLATION_RATE (20.0f)

/** Time in between physics steps. */
#define DK_PHYSICS_STEP_RATE (1.0f/30.0f)

/** Linear sleeping threshold for physics bodies. */
#define DK_PHYSICS_LINEAR_SLEEP_THRESHOLD 0.025f

/** Angular sleeping threshold for physics bodies. */
#define DK_PHYSICS_ANGULAR_SLEEP_THRESHOLD 0.01f