#pragma once

/**
 * @file editor_window.hpp
 * @brief Editor window header file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "imgui\imgui.h"
#include "imgui\imguidock.h"
#include "graphics\graphics.hpp"
#include "graphics\forward_renderer.hpp"
#include "input.hpp"
#include "ecs\scene.hpp"
#include "editor_renderer.hpp"
#include "hierarchy.hpp"
#include "inspector.hpp"
#include "scene_view.hpp"
#include "file_explorer.hpp"
#include "toolbar.hpp"

namespace dk
{
	/**
	 * Primary editor window.
	 */
	class EditorWindow
	{
	public:
		
		/**
		 * Constructor.
		 * @param Graphics context.
		 * @param Editor renderer.
		 * @param Scene renderer.
		 * @param Input manager.
		 * @param Scene.
		 * @param Resource manager.
		 */
		EditorWindow
		(
			Graphics* graphics, 
			EditorRenderer* editor_renderer, 
			OffScreenForwardRenderer* scene_renderer, 
			Input* input, 
			Scene* scene,
			ResourceManager* resource_manager
		);

		/**
		 * Destructor.
		 */
		~EditorWindow();

		/**
		 * Draw the window.
		 * @param Time since last draw.
		 */
		void draw(float dt);

		/**
		 * Get inspector.
		 * @return Inspector.
		 */
		Inspector& get_inspector() const
		{
			return *m_inspector.get();
		}

		/**
		 * Get hierarchy.
		 * @return Hierarchy.
		 */
		EditorHierarchy& get_hierarchy() const
		{
			return *m_hierarchy.get();
		}

		/**
		 * Get scene view.
		 * @return Scene view.
		 */
		SceneView& get_scene_view() const
		{
			return *m_scene_view.get();
		}

		/**
		 * Get tool bar.
		 * @return Tool bar.
		 */
		Toolbar& get_toolbar() const
		{
			return *m_toolbar.get();
		}

		/**
		 * Get file explorer.
		 * @return File explorer.
		 */
		FileExplorer& get_file_explorer() const
		{
			return *m_file_explorer.get();
		}

	private:

		/** Graphics context. */
		Graphics* m_graphics;

		/** Editor renderer. */
		EditorRenderer* m_editor_renderer;

		/** Scene renderer. */
		OffScreenForwardRenderer* m_scene_renderer;

		/** Input manager. */
		Input* m_input;

		/** Scene. */
		Scene* m_scene;

		/** Resource manager. */
		ResourceManager* m_resource_manager;

		/** Font texture. */
		std::unique_ptr<Texture> m_font_texture;

		/** Font descriptor pool. */
		vk::DescriptorPool m_vk_font_descriptor_pool;

		/** Font descriptor set. */
		vk::DescriptorSet m_vk_font_descriptor_set;

		/** Inspector widget. */
		std::unique_ptr<Inspector> m_inspector;

		/** Hierarchy widget. */
		std::unique_ptr<EditorHierarchy> m_hierarchy;

		/** Scene view widget. */
		std::unique_ptr<SceneView> m_scene_view;

		/** Toolbar widget. */
		std::unique_ptr<Toolbar> m_toolbar;

		/** File explorer widget. */
		std::unique_ptr<FileExplorer> m_file_explorer;

		/** Cursor handles. */
		std::array<SDL_Cursor*, ImGuiMouseCursor_COUNT> m_cursors = {};

		/** Dock context. */
		ImGui::DockContext* m_dock_context;
	};
}