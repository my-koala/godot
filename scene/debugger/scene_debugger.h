/**************************************************************************/
/*  scene_debugger.h                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "core/input/shortcut.h"
#include "core/object/ref_counted.h"
#include "core/string/ustring.h"
#include "core/templates/pair.h"
#include "core/variant/array.h"
#include "scene/gui/view_panner.h"
#ifndef _3D_DISABLED
#include "scene/resources/mesh.h"
#endif // _3D_DISABLED

class CanvasItem;
class LiveEditor;
class PopupMenu;
class RuntimeNodeSelect;
class Script;
class SceneTree;
#ifndef _3D_DISABLED
class Node3D;
#endif // _3D_DISABLED

class SceneDebugger {
private:
	inline static SceneDebugger *singleton = nullptr;

	SceneDebugger();

public:
	static void initialize();
	static void deinitialize();

	~SceneDebugger();

#ifdef DEBUG_ENABLED
private:
	static void _handle_input(const Ref<InputEvent> &p_event, const Ref<Shortcut> &p_shortcut);
	static void _handle_embed_input(const Ref<InputEvent> &p_event, const Dictionary &p_settings);

	static void _save_node(ObjectID id, const String &p_path);
	static void _set_node_owner_recursive(Node *p_node, Node *p_owner);
	static void _set_object_property(ObjectID p_id, const String &p_property, const Variant &p_value, const String &p_field = "");
	static void _send_object_ids(const Vector<ObjectID> &p_ids, bool p_update_selection);
	static void _next_frame();

	/// Message handler function for parse_message.
	typedef Error (*ParseMessageFunc)(const Array &p_args);
	static HashMap<String, ParseMessageFunc> message_handlers;
	static void _init_message_handlers();

	static Error _msg_setup_scene(const Array &p_args);
	static Error _msg_setup_embedded_shortcuts(const Array &p_args);
	static Error _msg_request_scene_tree(const Array &p_args);
	static Error _msg_save_node(const Array &p_args);
	static Error _msg_inspect_objects(const Array &p_args);
	static Error _msg_clear_selection(const Array &p_args);
	static Error _msg_suspend_changed(const Array &p_args);
	static Error _msg_next_frame(const Array &p_args);
	static Error _msg_debug_mute_audio(const Array &p_args);
	static Error _msg_override_cameras(const Array &p_args);
	static Error _msg_transform_camera_2d(const Array &p_args);
#ifndef _3D_DISABLED
	static Error _msg_transform_camera_3d(const Array &p_args);
#endif
	static Error _msg_set_object_property(const Array &p_args);
	static Error _msg_set_object_property_field(const Array &p_args);
	static Error _msg_reload_cached_files(const Array &p_args);
	static Error _msg_live_set_root(const Array &p_args);
	static Error _msg_live_node_path(const Array &p_args);
	static Error _msg_live_res_path(const Array &p_args);
	static Error _msg_live_node_prop_res(const Array &p_args);
	static Error _msg_live_node_prop(const Array &p_args);
	static Error _msg_live_res_prop_res(const Array &p_args);
	static Error _msg_live_res_prop(const Array &p_args);
	static Error _msg_live_node_call(const Array &p_args);
	static Error _msg_live_res_call(const Array &p_args);
	static Error _msg_live_create_node(const Array &p_args);
	static Error _msg_live_instantiate_node(const Array &p_args);
	static Error _msg_live_remove_node(const Array &p_args);
	static Error _msg_live_remove_and_keep_node(const Array &p_args);
	static Error _msg_live_restore_node(const Array &p_args);
	static Error _msg_live_duplicate_node(const Array &p_args);
	static Error _msg_live_reparent_node(const Array &p_args);
	static Error _msg_runtime_node_select_setup(const Array &p_args);
	static Error _msg_runtime_node_select_set_type(const Array &p_args);
	static Error _msg_runtime_node_select_set_mode(const Array &p_args);
	static Error _msg_runtime_node_select_set_visible(const Array &p_args);
	static Error _msg_runtime_node_select_reset_camera_2d(const Array &p_args);
#ifndef _3D_DISABLED
	static Error _msg_runtime_node_select_reset_camera_3d(const Array &p_args);
#endif
	static Error _msg_rq_screenshot(const Array &p_args);

public:
	static Error parse_message(void *p_user, const String &p_msg, const Array &p_args, bool &r_captured);
	static void add_to_cache(const String &p_filename, Node *p_node);
	static void remove_from_cache(const String &p_filename, Node *p_node);
	static void reload_cached_files(const PackedStringArray &p_files);
#endif
};

#ifdef DEBUG_ENABLED
class SceneDebuggerObject {
private:
	void _parse_script_properties(Script *p_script, ScriptInstance *p_instance);

public:
	typedef Pair<PropertyInfo, Variant> SceneDebuggerProperty;
	ObjectID id;
	String class_name;
	List<SceneDebuggerProperty> properties;

	SceneDebuggerObject(ObjectID p_id);
	SceneDebuggerObject() {}

	void serialize(Array &r_arr, int p_max_size = 1 << 20);
	void deserialize(const Array &p_arr);
};

class SceneDebuggerTree {
public:
	struct RemoteNode {
		int child_count = 0;
		String name;
		String type_name;
		ObjectID id;
		String scene_file_path;
		uint8_t view_flags = 0;

		enum ViewFlags {
			VIEW_HAS_VISIBLE_METHOD = 1 << 1,
			VIEW_VISIBLE = 1 << 2,
			VIEW_VISIBLE_IN_TREE = 1 << 3,
		};

		RemoteNode(int p_child, const String &p_name, const String &p_type, ObjectID p_id, const String p_scene_file_path, int p_view_flags) {
			child_count = p_child;
			name = p_name;
			type_name = p_type;
			id = p_id;

			scene_file_path = p_scene_file_path;
			view_flags = p_view_flags;
		}

		RemoteNode() {}
	};

	List<RemoteNode> nodes;

	void serialize(Array &r_arr);
	void deserialize(const Array &p_arr);
	SceneDebuggerTree(Node *p_root);
	SceneDebuggerTree() {}
};

class LiveEditor {
private:
	friend class SceneDebugger;
	HashMap<int, NodePath> live_edit_node_path_cache;
	HashMap<int, String> live_edit_resource_cache;

	NodePath live_edit_root;
	String live_edit_scene;

	HashMap<String, HashSet<Node *>> live_scene_edit_cache;
	HashMap<Node *, HashMap<ObjectID, Node *>> live_edit_remove_list;

	void _send_tree();

	void _node_path_func(const NodePath &p_path, int p_id);
	void _res_path_func(const String &p_path, int p_id);

	void _node_set_func(int p_id, const StringName &p_prop, const Variant &p_value);
	void _node_set_res_func(int p_id, const StringName &p_prop, const String &p_value);
	void _node_call_func(int p_id, const StringName &p_method, const Variant **p_args, int p_argcount);
	void _res_set_func(int p_id, const StringName &p_prop, const Variant &p_value);
	void _res_set_res_func(int p_id, const StringName &p_prop, const String &p_value);
	void _res_call_func(int p_id, const StringName &p_method, const Variant **p_args, int p_argcount);
	void _root_func(const NodePath &p_scene_path, const String &p_scene_from);

	void _create_node_func(const NodePath &p_parent, const String &p_type, const String &p_name);
	void _instance_node_func(const NodePath &p_parent, const String &p_path, const String &p_name);
	void _remove_node_func(const NodePath &p_at);
	void _remove_and_keep_node_func(const NodePath &p_at, ObjectID p_keep_id);
	void _restore_node_func(ObjectID p_id, const NodePath &p_at, int p_at_pos);
	void _duplicate_node_func(const NodePath &p_at, const String &p_new_name);
	void _reparent_node_func(const NodePath &p_at, const NodePath &p_new_place, const String &p_new_name, int p_at_pos);

	LiveEditor() {
		singleton = this;
		live_edit_root = NodePath("/root");
	}

	inline static LiveEditor *singleton = nullptr;

public:
	static LiveEditor *get_singleton();
};

class RuntimeNodeSelect : public Object {
	GDCLASS(RuntimeNodeSelect, Object);

public:
	enum NodeType {
		NODE_TYPE_NONE,
		NODE_TYPE_2D,
		NODE_TYPE_3D,
		NODE_TYPE_MAX,
	};

	enum SelectMode {
		SELECT_MODE_SINGLE,
		SELECT_MODE_LIST,
		SELECT_MODE_MAX,
	};

private:
	friend class SceneDebugger;

	NodeType node_select_type = NODE_TYPE_2D;
	SelectMode node_select_mode = SELECT_MODE_SINGLE;

	struct SelectResult {
		Node *item = nullptr;
		real_t order = 0;
		_FORCE_INLINE_ bool operator<(const SelectResult &p_rr) const { return p_rr.order < order; }
	};

	const int SELECTION_MIN_AREA = 8 * 8;
	enum SelectionDragState {
		SELECTION_DRAG_NONE,
		SELECTION_DRAG_MOVE,
		SELECTION_DRAG_END,
	};
	SelectionDragState selection_drag_state = SELECTION_DRAG_NONE;

	bool has_selection = false;
	int max_selection = 1;
	Point2 selection_position = Point2(Math::INF, Math::INF);
	Rect2 selection_drag_area;
	PopupMenu *selection_list = nullptr;
	Color selection_area_fill;
	Color selection_area_outline;
	bool selection_visible = true;
	bool selection_update_queued = false;

	bool multi_shortcut_pressed = false;
	bool list_shortcut_pressed = false;
	RID draw_canvas;
	RID sel_drag_ci;

	bool camera_override = false;

	// Values taken from EditorZoomWidget.
	const float VIEW_2D_MIN_ZOOM = 1.0 / 128;
	const float VIEW_2D_MAX_ZOOM = 128;

	Ref<ViewPanner> panner;
	Vector2 view_2d_offset;
	real_t view_2d_zoom = 1.0;
	bool warped_panning = false;

	LocalVector<ObjectID> selected_ci_nodes;
	real_t sel_2d_grab_dist = 0;

	RID sbox_2d_ci;

#ifndef _3D_DISABLED
	struct Cursor {
		Vector3 pos;
		real_t x_rot, y_rot, distance, fov_scale;
		Vector3 eye_pos; // Used in freelook mode.

		Cursor() {
			// These rotations place the camera in +X +Y +Z, aka south east, facing north west.
			x_rot = 0.5;
			y_rot = -0.5;
			distance = 4;
			fov_scale = 1.0;
		}
	};
	Cursor cursor;

	// Values taken from Node3DEditor.
	const float VIEW_3D_MIN_ZOOM = 0.01;
#ifdef REAL_T_IS_DOUBLE
	const double VIEW_3D_MAX_ZOOM = 1'000'000'000'000;
#else
	const float VIEW_3D_MAX_ZOOM = 10'000;
#endif // REAL_T_IS_DOUBLE

	const float CAMERA_MIN_FOV_SCALE = 0.1;
	const float CAMERA_MAX_FOV_SCALE = 2.5;

	bool camera_first_override = true;
	bool camera_freelook = false;

	real_t camera_fov = 0;
	real_t camera_znear = 0;
	real_t camera_zfar = 0;

	bool invert_x_axis = false;
	bool invert_y_axis = false;
	bool warped_mouse_panning_3d = false;

	real_t freelook_base_speed = 0;
	real_t freelook_sensitivity = 0;
	real_t orbit_sensitivity = 0;
	real_t translation_sensitivity = 0;

	Vector2 previous_mouse_position;

	struct SelectionBox3D : public RefCounted {
		RID instance;
		RID instance_ofs;
		RID instance_xray;
		RID instance_xray_ofs;

		Transform3D transform;
		AABB bounds;

		~SelectionBox3D() {
			if (instance.is_valid()) {
				RS::get_singleton()->free(instance);
				RS::get_singleton()->free(instance_ofs);
				RS::get_singleton()->free(instance_xray);
				RS::get_singleton()->free(instance_xray_ofs);
			}
		}
	};
	HashMap<ObjectID, Ref<SelectionBox3D>> selected_3d_nodes;

	Color sbox_3d_color;
	Ref<ArrayMesh> sbox_3d_mesh;
	Ref<ArrayMesh> sbox_3d_mesh_xray;
	RID sbox_3d;
	RID sbox_3d_ofs;
	RID sbox_3d_xray;
	RID sbox_3d_xray_ofs;
#endif // _3D_DISABLED

	void _setup(const Dictionary &p_settings);

	void _node_set_type(NodeType p_type);
	void _select_set_mode(SelectMode p_mode);

	void _set_camera_override_enabled(bool p_enabled);

	void _root_window_input(const Ref<InputEvent> &p_event);
	void _items_popup_index_pressed(int p_index, PopupMenu *p_popup);
	void _update_input_state();

	void _process_frame();
	void _physics_frame();

	void _send_ids(const Vector<Node *> &p_picked_nodes, bool p_invert_new_selections = true);
	void _set_selected_nodes(const Vector<Node *> &p_nodes);
	void _queue_selection_update();
	void _update_selection();
	void _clear_selection();
	void _update_selection_drag(const Point2 &p_end_pos = Point2());
	void _set_selection_visible(bool p_visible);

	void _open_selection_list(const Vector<SelectResult> &p_items, const Point2 &p_pos);
	void _close_selection_list();

	void _find_canvas_items_at_pos(const Point2 &p_pos, Node *p_node, Vector<SelectResult> &r_items, const Transform2D &p_parent_xform = Transform2D(), const Transform2D &p_canvas_xform = Transform2D());
	void _find_canvas_items_at_rect(const Rect2 &p_rect, Node *p_node, Vector<SelectResult> &r_items, const Transform2D &p_parent_xform = Transform2D(), const Transform2D &p_canvas_xform = Transform2D());
	void _pan_callback(Vector2 p_scroll_vec, Ref<InputEvent> p_event);
	void _zoom_callback(float p_zoom_factor, Vector2 p_origin, Ref<InputEvent> p_event);
	void _reset_camera_2d();
	void _update_view_2d();

#ifndef _3D_DISABLED
	void _find_3d_items_at_pos(const Point2 &p_pos, Vector<SelectResult> &r_items);
	void _find_3d_items_at_rect(const Rect2 &p_rect, Vector<SelectResult> &r_items);
	Vector3 _get_screen_to_space(const Vector3 &p_vector3);

	bool _handle_3d_input(const Ref<InputEvent> &p_event);
	void _set_camera_freelook_enabled(bool p_enabled);
	void _cursor_scale_distance(real_t p_scale);
	void _scale_freelook_speed(real_t p_scale);
	void _cursor_look(Ref<InputEventWithModifiers> p_event);
	void _cursor_pan(Ref<InputEventWithModifiers> p_event);
	void _cursor_orbit(Ref<InputEventWithModifiers> p_event);
	Point2 _get_warped_mouse_motion(const Ref<InputEventMouseMotion> &p_event, Rect2 p_border) const;
	Transform3D _get_cursor_transform();
	void _reset_camera_3d();
#endif // _3D_DISABLED

	RuntimeNodeSelect() { singleton = this; }

	inline static RuntimeNodeSelect *singleton = nullptr;

public:
	static RuntimeNodeSelect *get_singleton();

	~RuntimeNodeSelect();
};
#endif // DEBUG_ENABLED
