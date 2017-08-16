/*
    Qalculate (GTK+ UI)

    Copyright (C) 2003-2007, 2008, 2016  Hanna Knutsson (hanna.knutsson@protonmail.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
*/


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "support.h"
#include "callbacks.h"
#include "interface.h"
#include "main.h"

#include <deque>

extern GtkBuilder *main_builder, *argumentrules_builder, *csvimport_builder, *csvexport_builder, *datasetedit_builder, *datasets_builder, *setbase_builder, *decimals_builder;
extern GtkBuilder *functionedit_builder, *functions_builder, *matrixedit_builder, *matrix_builder, *namesedit_builder, *nbases_builder, *plot_builder, *precision_builder;
extern GtkBuilder *preferences_builder, *unit_builder, *unitedit_builder, *units_builder, *unknownedit_builder, *variableedit_builder, *variables_builder;
extern GtkBuilder *periodictable_builder, *simplefunctionedit_builder, *percentage_builder;
extern vector<mode_struct> modes;

GtkWidget *mainwindow;

GtkWidget *tFunctionCategories;
GtkWidget *tFunctions;
GtkListStore *tFunctions_store;
GtkTreeStore *tFunctionCategories_store;

GtkWidget *tVariableCategories;
GtkWidget *tVariables;
GtkListStore *tVariables_store;
GtkTreeStore *tVariableCategories_store;

GtkWidget *tUnitCategories;
GtkWidget *tUnits;
GtkListStore *tUnits_store;
GtkTreeStore *tUnitCategories_store;

GtkWidget *tUnitSelectorCategories;
GtkWidget *tUnitSelector;
GtkListStore *tUnitSelector_store;
GtkTreeStore *tUnitSelectorCategories_store;

GtkWidget *tDatasets;
GtkWidget *tDataObjects;
GtkListStore *tDatasets_store;
GtkListStore *tDataObjects_store;

GtkWidget *tDataProperties;
GtkListStore *tDataProperties_store;

GtkWidget *tNames;
GtkListStore *tNames_store;

GtkWidget *tabs, *expander_keypad, *expander_history, *expander_stack, *expander_convert;
GtkEntryCompletion *completion;
GtkWidget *completion_view, *completion_window, *completion_scrolled;
GtkTreeModel *completion_filter;
GtkListStore *completion_store;

GtkWidget *tFunctionArguments;
GtkListStore *tFunctionArguments_store;
GtkWidget *tSubfunctions;
GtkListStore *tSubfunctions_store;

GtkWidget *tPlotFunctions;
GtkListStore *tPlotFunctions_store;

GtkWidget *tMatrixEdit, *tMatrix;
GtkListStore *tMatrixEdit_store, *tMatrix_store;
extern vector<GtkTreeViewColumn*> matrix_edit_columns, matrix_columns;

GtkCellRenderer *renderer, *history_renderer, *history_index_renderer, *ans_renderer, *register_renderer;
GtkTreeViewColumn *column, *register_column, *history_column, *history_index_column;
GtkTreeSelection *selection;

GtkWidget *expressiontext;
GtkTextBuffer *expressionbuffer;
extern GtkTextIter current_object_start, current_object_end;
GtkWidget *resultview;
GtkWidget *historyview;
GtkListStore *historystore;
GtkWidget *stackview;
GtkListStore *stackstore;
GtkWidget *statuslabel_l, *statuslabel_r;
GtkWidget *f_menu ,*v_menu, *u_menu, *u_menu2, *recent_menu;
GtkAccelGroup *accel_group;

GtkCssProvider *expression_provider, *resultview_provider, *statuslabel_l_provider, *statuslabel_r_provider;

extern bool show_keypad, show_history, show_stack, show_convert, continuous_conversion, set_missing_prefixes;
extern bool save_mode_on_exit, save_defs_on_exit, load_global_defs, hyp_is_on, inv_is_on, fetch_exchange_rates_at_startup, allow_multiple_instances;
extern bool display_expression_status, enable_completion;
extern bool use_custom_result_font, use_custom_expression_font, use_custom_status_font;
extern string custom_result_font, custom_expression_font, custom_status_font;
extern string status_error_color, status_warning_color;
extern bool status_error_color_set, status_warning_color_set;
extern int auto_update_exchange_rates;

extern PrintOptions printops;
extern EvaluationOptions evalops;

extern bool rpn_mode, rpn_keypad_only;

extern vector<GtkWidget*> mode_items;
extern vector<GtkWidget*> popup_result_mode_items;

extern deque<string> inhistory;
extern deque<int> inhistory_type;

gint win_height, win_width, history_height, keypad_height, variables_width, variables_height, variables_position, units_width, units_height, units_position, functions_width, functions_height, functions_hposition, functions_vposition, datasets_width, datasets_height, datasets_hposition, datasets_vposition1, datasets_vposition2;

extern Unit *latest_button_unit, *latest_button_currency;
extern string latest_button_unit_pre, latest_button_currency_pre;

bool rpn_off_accelerator_set;

gchar history_error_color[8];
gchar history_warning_color[8];
gchar history_parse_color[8];

extern string fix_history_string(const string &str);

gint compare_categories(gconstpointer a, gconstpointer b) {
	return strcasecmp((const char*) a, (const char*) b);
}

void set_assumptions_items(AssumptionType at, AssumptionSign as) {	
	switch(as) {
		case ASSUMPTION_SIGN_POSITIVE: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_positive")), TRUE); break;}
		case ASSUMPTION_SIGN_NONPOSITIVE: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_nonpositive")), TRUE); break;}
		case ASSUMPTION_SIGN_NEGATIVE: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_negative")), TRUE); break;}
		case ASSUMPTION_SIGN_NONNEGATIVE: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_nonnegative")), TRUE); break;}
		case ASSUMPTION_SIGN_NONZERO: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_nonzero")), TRUE); break;}
		default: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_unknown")), TRUE);}
	}
	switch(at) {
		case ASSUMPTION_TYPE_INTEGER: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_integer")), TRUE); break;}
		case ASSUMPTION_TYPE_RATIONAL: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_rational")), TRUE); break;}
		case ASSUMPTION_TYPE_REAL: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_real")), TRUE); break;}
		case ASSUMPTION_TYPE_COMPLEX: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_complex")), TRUE); break;}
		case ASSUMPTION_TYPE_NUMBER: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_number")), TRUE); break;}
		case ASSUMPTION_TYPE_NONMATRIX: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_nonmatrix")), TRUE); break;}
		default: {gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assumptions_none")), TRUE);}
	}
}

void set_mode_items(const PrintOptions &po, const EvaluationOptions &eo, AssumptionType at, AssumptionSign as, bool in_rpn_mode, int precision, bool initial_update) {

	if(in_rpn_mode && evalops.parse_options.rpn) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_rpn_on")), true);
	else if(in_rpn_mode) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_rpn_stack")), true);
	else if(evalops.parse_options.rpn) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_rpn_syntax")), true);
	else gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_rpn_off")), true);
	
	if(in_rpn_mode && evalops.parse_options.rpn) {
		gtk_widget_add_accelerator(GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_item_rpn_off")), "activate", accel_group, GDK_KEY_R, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
		rpn_off_accelerator_set = true;
	} else {
		gtk_widget_add_accelerator(GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_item_rpn_on")), "activate", accel_group, GDK_KEY_R, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
		rpn_off_accelerator_set = false;
	}

	switch(eo.approximation) {
		case APPROXIMATION_EXACT: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_always_exact")), TRUE);
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_approximation")), 0);
			break;
		}
		case APPROXIMATION_TRY_EXACT: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_try_exact")), TRUE);
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_approximation")), 1);
			break;
		}
		case APPROXIMATION_APPROXIMATE: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_approximate")), TRUE);
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_approximation")), 2);
			break;
		}
	}
	
	switch(eo.auto_post_conversion) {
		case POST_CONVERSION_OPTIMAL: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_post_conversion_optimal")), TRUE);
			break;
		}
		case POST_CONVERSION_OPTIMAL_SI: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_post_conversion_optimal_si")), TRUE);
			break;
		}
		case POST_CONVERSION_BASE: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_post_conversion_base")), TRUE);
			break;
		}
		default: {			
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_post_conversion_none")), TRUE);
			break;
		}
	}

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_mixed_units_conversion")), eo.mixed_units_conversion != MIXED_UNITS_CONVERSION_NONE);
	
	switch(eo.parse_options.angle_unit) {
		case ANGLE_UNIT_DEGREES: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_degrees")), TRUE);
			//if(initial_update) gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object(main_builder, "radiobutton_degrees")), TRUE);
			break;
		}
		case ANGLE_UNIT_RADIANS: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_radians")), TRUE);
			//if(initial_update) gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object(main_builder, "radiobutton_radians")), TRUE);
			break;
		}
		case ANGLE_UNIT_GRADIANS: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_gradians")), TRUE);
			//if(initial_update) gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object(main_builder, "radiobutton_gradians")), TRUE);
			break;
		}
		default: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_no_default_angle_unit")), TRUE);
			//if(initial_update) gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object(main_builder, "radiobutton_no_default_angle_unit")), TRUE);
			break;
		}
	}

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_read_precision")), eo.parse_options.read_precision != DONT_READ_PRECISION);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_limit_implicit_multiplication")), eo.parse_options.limit_implicit_multiplication);
	switch(eo.parse_options.parsing_mode) {
		case PARSING_MODE_IMPLICIT_MULTIPLICATION_FIRST: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_ignore_whitespace")), TRUE);
			break;
		}
		case PARSING_MODE_CONVENTIONAL: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_no_special_implicit_multiplication")), TRUE);
			break;
		 }
		 default: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_adaptive_parsing")), TRUE);
			break;
		}
	}
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_assume_nonzero_denominators")), eo.assume_denominators_nonzero);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_warn_about_denominators_assumed_nonzero")), eo.warn_about_denominators_assumed_nonzero);
	
	switch(eo.structuring) {
		case STRUCTURING_HYBRID: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_algebraic_mode_hybrid")), TRUE);
			break;
		}
		case STRUCTURING_FACTORIZE: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_algebraic_mode_factorize")), TRUE);
			break;
		}
		default: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_algebraic_mode_simplify")), TRUE);
			break;
		}
	}

	switch(po.base) {
		case BASE_BINARY: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_base")), 0);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_binary")), TRUE);
			break;
		}
		case BASE_OCTAL: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_base")), 1);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_octal")), TRUE);
			break;
		}
		case BASE_DECIMAL: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_base")), 2);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_decimal")), TRUE);
			break;
		}
		case BASE_HEXADECIMAL: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_base")), 3);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_hexadecimal")), TRUE);
			break;
		}
		case BASE_ROMAN_NUMERALS: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_base")), 6);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_roman")), TRUE);
			break;
		}
		case BASE_SEXAGESIMAL: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_base")), 4);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_sexagesimal")), TRUE);
			break;
		}
		case BASE_TIME: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_base")), 5);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_time_format")), TRUE);
			break;
		}
		default: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_base")), 7);
			if(initial_update) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_custom_base")), TRUE);			
			printops.base = po.base;
			output_base_updated_from_menu();
		}
	}
	
	switch(po.min_exp) {
		case EXP_PRECISION: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_numerical_display")), 0);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_display_normal")), TRUE);
			break;
		}
		case EXP_BASE_3: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_numerical_display")), 1);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_display_engineering")), TRUE);
			break;
		}
		case EXP_SCIENTIFIC: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_numerical_display")), 2);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_display_scientific")), TRUE);
			break;
		}
		case EXP_PURE: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_numerical_display")), 3);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_display_purely_scientific")), TRUE);
			break;
		}
		case EXP_NONE: {
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_numerical_display")), 4);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_display_non_scientific")), TRUE);
			break;
		}
	}

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_indicate_infinite_series")), po.indicate_infinite_series);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_show_ending_zeroes")), po.show_ending_zeroes);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_round_halfway_to_even")), po.round_halfway_to_even);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_negative_exponents")), po.negative_exponents);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_sort_minus_last")), po.sort_options.minus_last);	
	
	if(!po.use_unit_prefixes) {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_display_no_prefixes")), TRUE);
	} else if(po.use_prefixes_for_all_units) {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_display_prefixes_for_all_units")), TRUE);
	} else if(po.use_prefixes_for_currencies) {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_display_prefixes_for_currencies")), TRUE);
	} else {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_display_prefixes_for_selected_units")), TRUE);
	}
	
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_all_prefixes")), po.use_all_prefixes);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_denominator_prefixes")), po.use_denominator_prefix);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_place_units_separately")), po.place_units_separately);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_abbreviate_names")), po.abbreviate_names);
			
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_enable_variables")), eo.parse_options.variables_enabled);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_enable_functions")), eo.parse_options.functions_enabled);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_enable_units")), eo.parse_options.units_enabled);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_enable_unknown_variables")), eo.parse_options.unknowns_enabled);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_calculate_variables")), eo.calculate_variables);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_allow_complex")), eo.allow_complex);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_allow_infinite")), eo.allow_infinite);

	switch(po.number_fraction_format) {
		case FRACTION_DECIMAL: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_fraction_decimal")), TRUE);
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_fraction_mode")), 0);
			break;
		}
		case FRACTION_DECIMAL_EXACT: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_fraction_decimal_exact")), TRUE);
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_fraction_mode")), 1);
			break;
		}
		case FRACTION_COMBINED: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_fraction_combined")), TRUE);
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_fraction_mode")), 3);
			break;		
		}
		case FRACTION_FRACTIONAL: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_builder_get_object(main_builder, "menu_item_fraction_fraction")), TRUE);
			if(initial_update) gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(main_builder, "combobox_fraction_mode")), 2);
			break;		
		}
	}

	set_assumptions_items(at, as);
	
	if(!initial_update) {
		if(decimals_builder) {
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object(decimals_builder, "decimals_dialog_checkbutton_min")), po.use_min_decimals);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object(decimals_builder, "decimals_dialog_checkbutton_max")), po.use_max_decimals);	
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(decimals_builder, "decimals_dialog_spinbutton_min")), po.min_decimals);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(decimals_builder, "decimals_dialog_spinbutton_max")), po.max_decimals);
		} else {
			printops.max_decimals = po.max_decimals;
			printops.use_max_decimals = po.use_max_decimals;
			printops.max_decimals = po.min_decimals;
			printops.use_min_decimals = po.use_min_decimals;
		}
		if(precision_builder) {
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(precision_builder, "precision_dialog_spinbutton_precision")), precision);	
		} else {
			CALCULATOR->setPrecision(precision);
		}
		printops.spacious = po.spacious;
		printops.short_multiplication = po.short_multiplication;
		printops.excessive_parenthesis = po.excessive_parenthesis;
		evalops.calculate_functions = eo.calculate_functions;
		if(setbase_builder) {
			switch(eo.parse_options.base) {
				case BASE_BINARY: {
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_binary")), TRUE);
					break;
				}
				case BASE_OCTAL: {
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_octal")), TRUE);
					break;
				}
				case BASE_DECIMAL: {
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_decimal")), TRUE);
					break;
				}
				case BASE_HEXADECIMAL: {
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_hexadecimal")), TRUE);
					break;
				}
				case BASE_ROMAN_NUMERALS: {
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_roman")), TRUE);
					break;
				}
				default: {
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_other")), TRUE);
					gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_input_other")), eo.parse_options.base);
				}
			}
		} else {
			evalops.parse_options.base = eo.parse_options.base;
		}
	}

}

gboolean history_row_separator_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer) {
	gint hindex = -1;
	gtk_tree_model_get(model, iter, 1, &hindex, -1);
	return hindex < 0;
}

GtkBuilder *getBuilder(const char *filename) {
#ifndef WIN32
	gchar *gstr = g_build_filename (PACKAGE_DATA_DIR, PACKAGE, "ui", filename, NULL);
	GtkBuilder *builder = gtk_builder_new_from_file(gstr);
	g_free(gstr);
	return builder;
#else
	char exepath[MAX_PATH];
	GetModuleFileName(NULL, exepath, MAX_PATH);
	string datadir(exepath);
	datadir.resize(datadir.find_last_of('\\'));
	if(datadir.substr(datadir.length() - 4) == "\\bin") {
		datadir.resize(datadir.find_last_of('\\'));
		datadir += "\\share\\";
		datadir += PACKAGE;
		datadir += "\\ui";
	} else if(datadir.substr(datadir.length() - 6) == "\\.libs") {
		datadir.resize(datadir.find_last_of('\\'));
		datadir.resize(datadir.find_last_of('\\'));
		datadir += "\\data";
	} else {
		datadir += "\\ui";
	}
	datadir += "\\";
	datadir += filename;
	return gtk_builder_new_from_file(datadir.c_str());
#endif
}

void create_button_menus(void) {

	GtkWidget *item, *sub;
	Variable *v;
	MathFunction *f;
	
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_bases"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_shift->title(true).c_str(), insert_button_function, CALCULATOR->f_shift)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_xor->title(true).c_str(), insert_button_function, CALCULATOR->f_xor)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_bitxor->title(true).c_str(), insert_button_function, CALCULATOR->f_bitxor)
	
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_percent"));
	MENU_ITEM(_("Percentage calculation tool"), on_menu_item_show_percentage_dialog_activate)
	
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_xy"));
	MENU_ITEM_MARKUP("x<sup><small>2</small></sup>", on_button_square_clicked)
	f = CALCULATOR->getActiveFunction("inv");
	if(f) {MENU_ITEM_WITH_POINTER("1/x", insert_button_function, f);}
	MENU_ITEM_MARKUP_WITH_POINTER("e<sup><small>x</small></sup>", insert_button_function, CALCULATOR->f_exp)
	f = CALCULATOR->getActiveFunction("exp2");
	if(f) {MENU_ITEM_MARKUP_WITH_POINTER("2<sup><small>x</small></sup>", insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("exp10");
	if(f) {MENU_ITEM_MARKUP_WITH_POINTER("10<sup><small>x</small></sup>", insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("cis");
	if(f) {MENU_ITEM_MARKUP_WITH_POINTER("e<sup><small>xi</small></sup>", insert_button_function, f);}
	
	g_signal_connect(gtk_builder_get_object(main_builder, "button_sqrt"), "clicked", G_CALLBACK(insert_button_function), (gpointer) CALCULATOR->f_sqrt);
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_sqrt"));
	f = CALCULATOR->getActiveFunction("cbrt");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("root");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("sqrtpi");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	
	g_signal_connect(gtk_builder_get_object(main_builder, "button_ln"), "clicked", G_CALLBACK(insert_button_function), (gpointer) CALCULATOR->f_ln);
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_ln"));
	f = CALCULATOR->getActiveFunction("log10");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("log2");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_logn->title(true).c_str(), insert_button_function, CALCULATOR->f_logn)
	
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_fac"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_factorial2->title(true).c_str(), insert_button_function, CALCULATOR->f_factorial2)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_multifactorial->title(true).c_str(), insert_button_function, CALCULATOR->f_multifactorial)
	f = CALCULATOR->getActiveFunction("hyperfactorial");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("superfactorial");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("perm");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("comb");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("derangements");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_binomial->title(true).c_str(), insert_button_function, CALCULATOR->f_binomial)
	
	g_signal_connect(gtk_builder_get_object(main_builder, "button_mod"), "clicked", G_CALLBACK(insert_button_function), (gpointer) CALCULATOR->f_mod);
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_mod"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_rem->title(true).c_str(), insert_button_function, CALCULATOR->f_rem)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_abs->title(true).c_str(), insert_button_function, CALCULATOR->f_abs)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_gcd->title(true).c_str(), insert_button_function, CALCULATOR->f_gcd)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_lcm->title(true).c_str(), insert_button_function, CALCULATOR->f_lcm)
	
	g_signal_connect(gtk_builder_get_object(main_builder, "button_sine"), "clicked", G_CALLBACK(insert_button_function), (gpointer) CALCULATOR->f_sin);
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_sin"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_sinh->title(true).c_str(), insert_button_function, CALCULATOR->f_sinh)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_asin->title(true).c_str(), insert_button_function, CALCULATOR->f_asin)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_asinh->title(true).c_str(), insert_button_function, CALCULATOR->f_asinh)
	
	g_signal_connect(gtk_builder_get_object(main_builder, "button_cosine"), "clicked", G_CALLBACK(insert_button_function), (gpointer) CALCULATOR->f_cos);
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_cos"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_cosh->title(true).c_str(), insert_button_function, CALCULATOR->f_cosh)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_acos->title(true).c_str(), insert_button_function, CALCULATOR->f_acos)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_acosh->title(true).c_str(), insert_button_function, CALCULATOR->f_acosh)
	
	g_signal_connect(gtk_builder_get_object(main_builder, "button_tan"), "clicked", G_CALLBACK(insert_button_function), (gpointer) CALCULATOR->f_tan);
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_tan"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_tanh->title(true).c_str(), insert_button_function, CALCULATOR->f_tanh)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_atan->title(true).c_str(), insert_button_function, CALCULATOR->f_atan)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_atanh->title(true).c_str(), insert_button_function, CALCULATOR->f_atanh)
	
	g_signal_connect(gtk_builder_get_object(main_builder, "button_sum"), "clicked", G_CALLBACK(insert_button_function), (gpointer) CALCULATOR->f_sum);
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_sum"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_product->title(true).c_str(), insert_button_function, CALCULATOR->f_product)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_for->title(true).c_str(), insert_button_function, CALCULATOR->f_for)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_if->title(true).c_str(), insert_button_function, CALCULATOR->f_if)
	
	
	g_signal_connect(gtk_builder_get_object(main_builder, "button_mean"), "clicked", G_CALLBACK(insert_button_function), (gpointer) CALCULATOR->getActiveFunction("mean"));
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_mean"));
	f = CALCULATOR->getActiveFunction("median");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("var");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("stdev");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("stderr");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("harmmean");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("geomean");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	
	g_signal_connect(gtk_builder_get_object(main_builder, "button_pi"), "clicked", G_CALLBACK(insert_button_variable), (gpointer) CALCULATOR->v_pi);
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_pi"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->v_e->title(true).c_str(), insert_button_variable, CALCULATOR->v_e)
	v = CALCULATOR->getActiveVariable("euler");
	if(v) {MENU_ITEM_WITH_POINTER(v->title(true).c_str(), insert_button_variable, v);}
	v = CALCULATOR->getActiveVariable("golden");
	if(v) {MENU_ITEM_WITH_POINTER(v->title(true).c_str(), insert_button_variable, v);}
	v = CALCULATOR->getActiveVariable("pythagoras");
	if(v) {MENU_ITEM_WITH_POINTER(v->title(true).c_str(), insert_button_variable, v);}
	MENU_SEPARATOR
	v = CALCULATOR->getActiveVariable("c");
	if(v) {MENU_ITEM_WITH_POINTER(v->title(true).c_str(), insert_button_variable, v);}
	v = CALCULATOR->getActiveVariable("newtonian_constant");
	if(v) {MENU_ITEM_WITH_POINTER(v->title(true).c_str(), insert_button_variable, v);}
	v = CALCULATOR->getActiveVariable("planck");
	if(v) {MENU_ITEM_WITH_POINTER(v->title(true).c_str(), insert_button_variable, v);}
	v = CALCULATOR->getActiveVariable("boltzmann");
	if(v) {MENU_ITEM_WITH_POINTER(v->title(true).c_str(), insert_button_variable, v);}
	
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_factorize"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_solve->title(true).c_str(), insert_button_function, CALCULATOR->f_solve)
	f = CALCULATOR->getActiveFunction("solve2");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("linearfunction");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_diff->title(true).c_str(), insert_button_function, CALCULATOR->f_diff)
	f = CALCULATOR->getActiveFunction("extremum");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	MENU_SEPARATOR
	MENU_ITEM(_("Simplify"), on_menu_item_simplify_activate)
	MENU_ITEM(_("Set unknowns"), on_menu_item_set_unknowns_activate)
	
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_i"));
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_re->title(true).c_str(), insert_button_function, CALCULATOR->f_re)
	MENU_ITEM_WITH_POINTER(CALCULATOR->f_im->title(true).c_str(), insert_button_function, CALCULATOR->f_im)
	f = CALCULATOR->getActiveFunction("arg");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	f = CALCULATOR->getActiveFunction("conj");
	if(f) {MENU_ITEM_WITH_POINTER(f->title(true).c_str(), insert_button_function, f);}
	
	
	//g_signal_connect_data(gtk_builder_get_object(main_builder, "mb_to"), "activate", G_CALLBACK(on_mb_to_clicked), NULL);
	if(!latest_button_unit_pre.empty()) {
		latest_button_unit = CALCULATOR->getActiveUnit(latest_button_unit_pre);
		if(!latest_button_unit) latest_button_unit = CALCULATOR->getCompositeUnit(latest_button_unit_pre);
	}
	if(latest_button_unit) {
		string si_label_str;
		if(latest_button_unit->subtype() == SUBTYPE_COMPOSITE_UNIT) {
			si_label_str = ((CompositeUnit*) latest_button_unit)->print(false, true, printops.use_unicode_signs, &can_display_unicode_string_function, (void*) expressiontext);
		} else {
		
			si_label_str = latest_button_unit->preferredDisplayName(true, printops.use_unicode_signs, false, false, &can_display_unicode_string_function, (void*) expressiontext).name;
		}
		gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(main_builder, "label_si")), si_label_str.c_str());
		gtk_widget_set_tooltip_text(GTK_WIDGET(gtk_builder_get_object(main_builder, "label_si")), latest_button_unit->title(true).c_str());
	}
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_si"));
	const char *si_units[] = {"m", "g", "s", "A", "K", "N", "Pa", "J", "W", "L", "V", "ohm", "oC", "cd", "mol", "C", "Hz", "F", "S", "Wb", "T", "H", "lm", "lx", "Bq", "Gy", "Sv", "kat"};
	vector<Unit*> to_us;
	size_t si_i = 0;
	size_t i_added = 0;
	for(; si_i < 27 && i_added < 12; si_i++) {
		Unit * u = CALCULATOR->getActiveUnit(si_units[si_i]);
		if(u && !u->isHidden()) {
			bool b = false;
			for(size_t i2 = 0; i2 < to_us.size(); i2++) {
				if(u->title(true) < to_us[i2]->title(true)) {
					to_us.insert(to_us.begin() + i2, u);
					b = true;
					break;
				}
			}
			if(!b) to_us.push_back(u);
			i_added++;
		}
	}
	for(size_t i = 0; i < to_us.size(); i++) {
		MENU_ITEM_WITH_POINTER(to_us[i]->title(true).c_str(), insert_button_unit, to_us[i])
	}
	
	// Show further items in a submenu
	if(si_i < 27) {SUBMENU_ITEM(_("more"), sub);}
	
	to_us.clear();
	for(; si_i < 27; si_i++) {
		Unit * u = CALCULATOR->getActiveUnit(si_units[si_i]);
		if(u && !u->isHidden()) {
			bool b = false;
			for(size_t i2 = 0; i2 < to_us.size(); i2++) {
				if(u->title(true) < to_us[i2]->title(true)) {
					to_us.insert(to_us.begin() + i2, u);
					b = true;
					break;
				}
			}
			if(!b) to_us.push_back(u);
		}
	}
	for(size_t i = 0; i < to_us.size(); i++) {
		MENU_ITEM_WITH_POINTER(to_us[i]->title(true).c_str(), insert_button_unit, to_us[i])
	}
	/*i_added = 0;
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_si"));
	for(size_t i = 0; i < CALCULATOR->units.size(); i++) {
		if(!CALCULATOR->units[i]->isHidden() && CALCULATOR->units[i]->subtype() != SUBTYPE_COMPOSITE_UNIT && (CALCULATOR->units[i]->system() == "Imperial" || CALCULATOR->units[i]->system() == "US" || CALCULATOR->units[i]->system() == "Imperial/US")) {
			Unit *u = CALCULATOR->units[i];
			if(u->referenceName().find("UK_") == string::npos && u->referenceName().find("US_") == string::npos) {
				i_added++;
				if(i_added == 1) {SUBMENU_ITEM(_("Imperial/US"), sub);}
				// Show further items in a submenu
				else if(i_added % 10 == 0) {SUBMENU_ITEM(_("more"), sub);}
				MENU_ITEM_WITH_POINTER(u->title(true).c_str(), insert_button_unit, u)
			}
		}
	}
	i_added = 0;
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_si"));
	for(size_t i = 0; i < CALCULATOR->units.size(); i++) {
		if(!CALCULATOR->units[i]->isHidden() && CALCULATOR->units[i]->subtype() != SUBTYPE_COMPOSITE_UNIT && CALCULATOR->units[i]->system() == "CGS") {
			Unit *u = CALCULATOR->units[i];
			i_added++;
			if(i_added == 1) {SUBMENU_ITEM("CGS", sub);}
			// Show further items in a submenu
			else if(i_added % 10 == 0) {SUBMENU_ITEM(_("more"), sub);}
			MENU_ITEM_WITH_POINTER(u->title(true).c_str(), insert_button_unit, u)
		}
	}*/
	

	if(!latest_button_currency_pre.empty()) {
		latest_button_currency = CALCULATOR->getActiveUnit(latest_button_currency_pre);
	}
	string local_currency;
	struct lconv *lc = localeconv();
	if(lc) local_currency = lc->int_curr_symbol;
	remove_blank_ends(local_currency);
	if(!latest_button_currency && !local_currency.empty()) latest_button_currency = CALCULATOR->getActiveUnit(local_currency);
	if(!latest_button_currency) latest_button_currency = CALCULATOR->u_euro;
	string unit_label_str;
	if(latest_button_currency->subtype() == SUBTYPE_COMPOSITE_UNIT) {
		unit_label_str = ((CompositeUnit*) latest_button_currency)->print(false, true, printops.use_unicode_signs, &can_display_unicode_string_function, (void*) expressiontext);
	} else {
	
		unit_label_str = latest_button_currency->preferredDisplayName(true, printops.use_unicode_signs, false, false, &can_display_unicode_string_function, (void*) expressiontext).name;
	}
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(main_builder, "label_euro")), unit_label_str.c_str());
	gtk_widget_set_tooltip_text(GTK_WIDGET(gtk_builder_get_object(main_builder, "label_euro")), latest_button_currency->title(true).c_str());
	sub = GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_euro"));
	const char *currency_units[] = {"USD", "GBP", "JPY"};
	to_us.clear();
	for(size_t i = 0; i < 5; i++) {
		Unit * u;
		if(i == 0) u = CALCULATOR->u_euro;
		else if(i == 1) u = CALCULATOR->getActiveUnit(local_currency);
		else u = CALCULATOR->getActiveUnit(currency_units[i - 2]);
		if(u && !u->isHidden()) {
			bool b = false;
			for(size_t i2 = 0; i2 < to_us.size(); i2++) {
				if(u == to_us[i2]) {
					b = true;
					break;
				}
				if(u->title(true) < to_us[i2]->title(true)) {
					to_us.insert(to_us.begin() + i2, u);
					b = true;
					break;
				}
			}
			if(!b) to_us.push_back(u);
		}
	}
	for(size_t i = 0; i < to_us.size(); i++) {
		MENU_ITEM_WITH_POINTER(to_us[i]->title(true).c_str(), insert_button_currency, to_us[i])
	}

	i_added = to_us.size();
	string s_cat = CALCULATOR->u_euro->category();
	if(!s_cat.empty()) {
		for(size_t i = 0; i < CALCULATOR->units.size(); i++) {
			if(CALCULATOR->units[i]->category() == s_cat) {
				Unit *u = CALCULATOR->units[i];
				if(u->isActive() && !u->isHidden() && u->isBuiltin()) {
					bool b = false;
					for(size_t i2 = 0; i2 < to_us.size(); i2++) {
						if(i2 < i_added && u == to_us[i2]) {
							b = true;
							break;
						}
						if(u->title(true) < to_us[i2]->title(true)) {
							to_us.insert(to_us.begin() + i2, u);
							b = true;
							break;
						}
					}
					if(!b) to_us.push_back(u);
				}
			}
		}
		for(size_t i = i_added; i < to_us.size(); i++) {
			// Show further items in a submenu
			if(i == i_added) {SUBMENU_ITEM(_("more"), sub);}
			MENU_ITEM_WITH_POINTER(to_us[i]->title(true).c_str(), insert_button_currency, to_us[i])
		}
	}

}

void create_main_window(void) {

	main_builder = getBuilder("main.ui");
	g_assert(main_builder != NULL);
	
	/* make sure we get a valid main window */
	g_assert(gtk_builder_get_object(main_builder, "main_window") != NULL);
	
	mainwindow = GTK_WIDGET(gtk_builder_get_object(main_builder, "main_window"));

	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(gtk_builder_get_object(main_builder, "main_window")), accel_group);

	if(win_width > 0) gtk_window_set_default_size (GTK_WINDOW(gtk_builder_get_object(main_builder, "main_window")), win_width, win_height);

#if GTK_MAJOR_VERSION > 3 || GTK_MINOR_VERSION >= 16
	gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(gtk_builder_get_object(main_builder, "scrolled_result")), false);
#endif

	gtk_menu_button_set_align_widget(GTK_MENU_BUTTON(gtk_builder_get_object(main_builder, "mb_sin")), GTK_WIDGET(gtk_builder_get_object(main_builder, "box_sin")));
	gtk_menu_button_set_align_widget(GTK_MENU_BUTTON(gtk_builder_get_object(main_builder, "mb_cos")), GTK_WIDGET(gtk_builder_get_object(main_builder, "box_cos")));
	gtk_menu_button_set_align_widget(GTK_MENU_BUTTON(gtk_builder_get_object(main_builder, "mb_tan")), GTK_WIDGET(gtk_builder_get_object(main_builder, "box_tan")));
	gtk_menu_button_set_align_widget(GTK_MENU_BUTTON(gtk_builder_get_object(main_builder, "mb_sqrt")), GTK_WIDGET(gtk_builder_get_object(main_builder, "box_sqrt")));
	gtk_menu_button_set_align_widget(GTK_MENU_BUTTON(gtk_builder_get_object(main_builder, "mb_xy")), GTK_WIDGET(gtk_builder_get_object(main_builder, "box_xy")));
	gtk_menu_button_set_align_widget(GTK_MENU_BUTTON(gtk_builder_get_object(main_builder, "mb_ln")), GTK_WIDGET(gtk_builder_get_object(main_builder, "box_ln")));
	gtk_menu_button_set_align_widget(GTK_MENU_BUTTON(gtk_builder_get_object(main_builder, "mb_sum")), GTK_WIDGET(gtk_builder_get_object(main_builder, "box_sum")));
	gtk_menu_button_set_align_widget(GTK_MENU_BUTTON(gtk_builder_get_object(main_builder, "mb_mean")), GTK_WIDGET(gtk_builder_get_object(main_builder, "box_mean")));
	gtk_menu_button_set_align_widget(GTK_MENU_BUTTON(gtk_builder_get_object(main_builder, "mb_pi")), GTK_WIDGET(gtk_builder_get_object(main_builder, "box_pi")));

	expressiontext = GTK_WIDGET(gtk_builder_get_object(main_builder, "expressiontext"));
	expressionbuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(main_builder, "expressionbuffer"));
	gtk_text_buffer_get_end_iter(expressionbuffer, &current_object_start);
	gtk_text_buffer_get_end_iter(expressionbuffer, &current_object_end);
	resultview = GTK_WIDGET(gtk_builder_get_object(main_builder, "resultview"));
	historyview = GTK_WIDGET(gtk_builder_get_object(main_builder, "historyview"));
	
#if GTK_MAJOR_VERSION > 3 || GTK_MINOR_VERSION >= 18
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(expressiontext), 6);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(expressiontext), 6);
	gtk_text_view_set_top_margin(GTK_TEXT_VIEW(expressiontext), 6);
	gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(expressiontext), 6);
#endif 
	
	stackview = GTK_WIDGET(gtk_builder_get_object(main_builder, "stackview"));
	statuslabel_l = GTK_WIDGET(gtk_builder_get_object(main_builder, "label_status_left"));
	statuslabel_r = GTK_WIDGET(gtk_builder_get_object(main_builder, "label_status_right"));

	expression_provider = gtk_css_provider_new();
	resultview_provider = gtk_css_provider_new();
	statuslabel_l_provider = gtk_css_provider_new();
	statuslabel_r_provider = gtk_css_provider_new();
	gtk_style_context_add_provider(gtk_widget_get_style_context(expressiontext), GTK_STYLE_PROVIDER(expression_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_style_context_add_provider(gtk_widget_get_style_context(resultview), GTK_STYLE_PROVIDER(resultview_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_style_context_add_provider(gtk_widget_get_style_context(statuslabel_l), GTK_STYLE_PROVIDER(statuslabel_l_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_style_context_add_provider(gtk_widget_get_style_context(statuslabel_r), GTK_STYLE_PROVIDER(statuslabel_r_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	
	set_mode_items(printops, evalops, CALCULATOR->defaultAssumptions()->type(), CALCULATOR->defaultAssumptions()->sign(), rpn_mode, CALCULATOR->getPrecision(), true);

	set_unicode_buttons();

	if(use_custom_result_font) {
		gchar *gstr = font_name_to_css(custom_result_font.c_str());
		gtk_css_provider_load_from_data(resultview_provider, gstr, -1, NULL);
		g_free(gstr);
	} else {
		//gtk_css_provider_load_from_data(resultview_provider, "* {font-size: large;}", -1, NULL);
		if(custom_result_font.empty()) {
			PangoFontDescription *font_desc;
			gtk_style_context_get(gtk_widget_get_style_context(resultview), GTK_STATE_FLAG_NORMAL, GTK_STYLE_PROPERTY_FONT, &font_desc, NULL);
			custom_result_font = pango_font_description_to_string(font_desc);
			pango_font_description_free(font_desc);
		}
	}
	if(use_custom_expression_font) {
		gchar *gstr = font_name_to_css(custom_expression_font.c_str());
		gtk_css_provider_load_from_data(expression_provider, gstr, -1, NULL);
		g_free(gstr);
	} else {
		//gtk_css_provider_load_from_data(expression_provider, "* {font-size: large;}", -1, NULL);
		if(custom_expression_font.empty()) {
			PangoFontDescription *font_desc;
			gtk_style_context_get(gtk_widget_get_style_context(expressiontext), GTK_STATE_FLAG_NORMAL, GTK_STYLE_PROPERTY_FONT, &font_desc, NULL);
			custom_expression_font = pango_font_description_to_string(font_desc);
			pango_font_description_free(font_desc);
		}
	}
	if(use_custom_status_font) {
		gchar *gstr = font_name_to_css(custom_status_font.c_str());
		gtk_css_provider_load_from_data(statuslabel_l_provider, gstr, -1, NULL);
		gtk_css_provider_load_from_data(statuslabel_l_provider, gstr, -1, NULL);
		g_free(gstr);
	} else {
		if(custom_status_font.empty()) {
			PangoFontDescription *font_desc;
			gtk_style_context_get(gtk_widget_get_style_context(statuslabel_l), GTK_STATE_FLAG_NORMAL, GTK_STYLE_PROPERTY_FONT, &font_desc, NULL);
			custom_status_font = pango_font_description_to_string(font_desc);
			pango_font_description_free(font_desc);
		}
	}
	GdkRGBA c;
	gtk_style_context_get_color(gtk_widget_get_style_context(statuslabel_l), GTK_STATE_FLAG_NORMAL, &c);
	if(!status_error_color_set) {
		GdkRGBA c_err = c;
		if(c_err.red >= 0.8) {
			c_err.green /= 1.5;
			c_err.blue /= 1.5;
			c_err.red = 1.0;
		} else {
			if(c_err.red >= 0.5) c_err.red = 1.0;
			else c_err.red += 0.5;
		}	
		gchar ecs[8];
		g_snprintf(ecs, 8, "#%02x%02x%02x", (int) (c_err.red * 255), (int) (c_err.green * 255), (int) (c_err.blue * 255));
		status_error_color = ecs;
	}
	
	if(!status_warning_color_set) {
		GdkRGBA c_warn = c;
		if(c_warn.blue >= 0.8) {
			c_warn.green /= 1.5;
			c_warn.red /= 1.5;
			c_warn.blue = 1.0;
		} else {
			if(c_warn.blue >= 0.3) c_warn.blue = 1.0;
			else c_warn.blue += 0.7;
		}
		gchar wcs[8];
		g_snprintf(wcs, 8, "#%02x%02x%02x", (int) (c_warn.red * 255), (int) (c_warn.green * 255), (int) (c_warn.blue * 255));
		status_warning_color = wcs;
	}
	
	gtk_widget_grab_focus(expressiontext);
	gtk_widget_set_can_default(expressiontext, TRUE);
	gtk_widget_grab_default(expressiontext);

	expander_keypad = GTK_WIDGET(gtk_builder_get_object(main_builder, "expander_keypad"));
	expander_history = GTK_WIDGET(gtk_builder_get_object(main_builder, "expander_history"));
	expander_stack = GTK_WIDGET(gtk_builder_get_object(main_builder, "expander_stack"));
	expander_convert = GTK_WIDGET(gtk_builder_get_object(main_builder, "expander_convert"));
	tabs = GTK_WIDGET(gtk_builder_get_object(main_builder, "tabs"));
	if(show_stack && rpn_mode) {
		gtk_expander_set_expanded(GTK_EXPANDER(expander_stack), TRUE);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), 1);
		gtk_widget_show(tabs);
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(main_builder, "buttons")));
	} else if(show_keypad) {
		gtk_expander_set_expanded(GTK_EXPANDER(expander_keypad), TRUE);
		gtk_widget_hide(tabs);
		gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(main_builder, "buttons")));
	} else if(show_history) {
		gtk_expander_set_expanded(GTK_EXPANDER(expander_history), TRUE);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), 0);
		gtk_widget_show(tabs);
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(main_builder, "buttons")));
	} else if(show_convert) {
		gtk_expander_set_expanded(GTK_EXPANDER(expander_convert), TRUE);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), 2);
		gtk_widget_show(tabs);
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(main_builder, "buttons")));
	} else {
		gtk_widget_hide(tabs);
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(main_builder, "buttons")));
	}
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(main_builder, "convert_button_continuous_conversion")), continuous_conversion);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(main_builder, "convert_button_set_missing_prefixes")), set_missing_prefixes);

	gtk_builder_connect_signals(main_builder, NULL);
	g_signal_connect(accel_group, "accel_changed", G_CALLBACK(save_accels), NULL);
	
	gtk_style_context_get_color(gtk_widget_get_style_context(historyview), GTK_STATE_FLAG_NORMAL, &c);
	GdkRGBA c_red = c;
	if(c_red.red >= 0.8) {
		c_red.green /= 1.5;
		c_red.blue /= 1.5;
		c_red.red = 1.0;
	} else {
		if(c_red.red >= 0.5) c_red.red = 1.0;
		else c_red.red += 0.5;
	}	
	g_snprintf(history_error_color, 8, "#%02x%02x%02x", (int) (c_red.red * 255), (int) (c_red.green * 255), (int) (c_red.blue * 255));
	
	GdkRGBA c_blue = c;
	if(c_blue.blue >= 0.8) {
		c_blue.green /= 1.5;
		c_blue.red /= 1.5;
		c_blue.blue = 1.0;
	} else {
		if(c_blue.blue >= 0.4) c_blue.blue = 1.0;
		else c_blue.blue += 0.6;
	}	
	g_snprintf(history_warning_color, 8, "#%02x%02x%02x", (int) (c_blue.red * 255), (int) (c_blue.green * 255), (int) (c_blue.blue * 255));
	
	GdkRGBA c_gray = c;
	if(c_gray.blue + c_gray.green + c_gray.red > 1.5) {
		c_gray.green /= 1.5;
		c_gray.red /= 1.5;
		c_gray.blue /= 1.5;
	} else if(c_gray.blue + c_gray.green + c_gray.red > 0.3) {
		c_gray.green += 0.175;
		c_gray.red += 0.175;
		c_gray.blue += 0.175;
	} else if(c_gray.blue + c_gray.green + c_gray.red > 0.15) {
		c_gray.green += 0.3;
		c_gray.red += 0.3;
		c_gray.blue += 0.3;
	} else {
		c_gray.green += 0.4;
		c_gray.red += 0.4;
		c_gray.blue += 0.4;
	}	
	g_snprintf(history_parse_color, 8, "#%02x%02x%02x", (int) (c_gray.red * 255), (int) (c_gray.green * 255), (int) (c_gray.blue * 255));
	
	historystore = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW(historyview), GTK_TREE_MODEL(historystore));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(historyview));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
	history_index_renderer = gtk_cell_renderer_text_new();
	history_index_column = gtk_tree_view_column_new_with_attributes(_("Index"), history_index_renderer, "text", 2, "ypad", 4, NULL);
	gtk_tree_view_column_set_expand(history_index_column, FALSE);
	gtk_tree_view_column_set_min_width(history_index_column, 30);
	g_object_set(G_OBJECT(history_index_renderer), "ypad", 0, "xalign", 0.5, "foreground-rgba", &c_gray, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(historyview), history_index_column);
	history_renderer = gtk_cell_renderer_text_new();
	history_column = gtk_tree_view_column_new_with_attributes(_("History"), history_renderer, "markup", 0, "ypad", 4, NULL);
	gtk_tree_view_column_set_expand(history_column, TRUE);
	g_object_set(G_OBJECT(history_renderer), "xpad", 6, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(historyview), history_column);
	g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_historyview_selection_changed), NULL);
	gtk_tree_view_set_row_separator_func(GTK_TREE_VIEW(historyview), history_row_separator_func, NULL, NULL);

	string history_str;
	GtkTreeIter history_iter;
	size_t i = inhistory.size();
	if(i == 0) gtk_list_store_insert_with_values(historystore, &history_iter, -1, 1, -1, 2, "   ", -1);
	while(i > 0) {
		i--;
		switch(inhistory_type[i]) {
			case QALCULATE_HISTORY_EXPRESSION: {
				if(i < inhistory.size() - 1) {
					gtk_list_store_insert_with_values(historystore, &history_iter, -1, 1, -1, -1);
				}
				gtk_list_store_insert_with_values(historystore, &history_iter, -1, 0, fix_history_string(inhistory[i]).c_str(), 1, i, 2, "   ", 4, EXPRESSION_YPAD, -1);
				break;
			}
			case QALCULATE_HISTORY_REGISTER_MOVED: {
				if(i < inhistory.size() - 1) {
					gtk_list_store_insert_with_values(historystore, &history_iter, -1, 1, -1, -1);
				}
				gtk_list_store_insert_with_values(historystore, &history_iter, -1, 0, _("RPN Register Moved"), 1, i, 2, "   ", 4, EXPRESSION_YPAD, -1);
				break;
			}
			case QALCULATE_HISTORY_RPN_OPERATION: {
				if(i < inhistory.size() - 1) {
					gtk_list_store_insert_with_values(historystore, &history_iter, -1, 1, -1, -1);
				}
				gtk_list_store_insert_with_values(historystore, &history_iter, -1, 0, _("RPN Operation"), 1, i, 2, "   ", 4, EXPRESSION_YPAD, -1);
				break;
			}
			case QALCULATE_HISTORY_TRANSFORMATION: {
				history_str = "<span font-style=\"italic\">";
				history_str += fix_history_string(inhistory[i]);
				history_str += ":  </span>";
				gtk_list_store_insert_with_values(historystore, &history_iter, -1, 0, history_str.c_str(), 1, i, 4, 0, -1);
				break;
			}
			case QALCULATE_HISTORY_RESULT: {
				if(i + 1 < inhistory.size()  && inhistory_type[i + 1] == QALCULATE_HISTORY_TRANSFORMATION) {
					gchar *expr_str = NULL;
					gtk_tree_model_get(GTK_TREE_MODEL(historystore), &history_iter, 0, &expr_str, -1);
					history_str = expr_str;
					g_free(expr_str);
					history_str += "= <span font-weight=\"bold\">";
					history_str += fix_history_string(inhistory[i]);
					history_str += "</span>";
					gtk_list_store_set(historystore, &history_iter, 0, history_str.c_str(), -1);
				} else {
					history_str = "= <span font-weight=\"bold\">";
					history_str += fix_history_string(inhistory[i]);
					history_str += "</span>";
					gtk_list_store_insert_with_values(historystore, &history_iter, -1, 0, history_str.c_str(), 1, i, 4, 0, -1);
				}
				break;
			}
			case QALCULATE_HISTORY_RESULT_APPROXIMATE: {
				string str;
				if(printops.use_unicode_signs && can_display_unicode_string_function(SIGN_ALMOST_EQUAL, (void*) historyview)) {
					str = SIGN_ALMOST_EQUAL " ";
				} else {
					str = "= ";
					str += _("approx.");
					str += " ";
				}
				if(i + 1 < inhistory.size() && inhistory_type[i + 1] == QALCULATE_HISTORY_TRANSFORMATION) {
					gchar *expr_str = NULL;
					gtk_tree_model_get(GTK_TREE_MODEL(historystore), &history_iter, 0, &expr_str, -1);
					history_str = expr_str;
					g_free(expr_str);
					history_str += str;
					history_str += "<span font-weight=\"bold\">";
					history_str += fix_history_string(inhistory[i]);
					history_str += "</span>";
					gtk_list_store_set(historystore, &history_iter, 0, history_str.c_str(), -1);
				} else {
					history_str = str;
					history_str += "<span font-weight=\"bold\">";
					history_str += fix_history_string(inhistory[i]);
					history_str += "</span>";
					gtk_list_store_insert_with_values(historystore, &history_iter, -1, 0, history_str.c_str(), 1, i, 4, 0, -1);
				}
				break;
			}
			case QALCULATE_HISTORY_PARSE_WITHEQUALS: {
				gchar *expr_str = NULL;
				gtk_tree_model_get(GTK_TREE_MODEL(historystore), &history_iter, 0, &expr_str, -1);
				history_str = expr_str;
				g_free(expr_str);
				history_str += "<span font-style=\"italic\" foreground=\"";
				history_str += history_parse_color;
				history_str += "\">  ";
				history_str += fix_history_string(inhistory[i]);
				history_str += "</span>";
				gtk_list_store_set(historystore, &history_iter, 0, history_str.c_str(), -1);
				if(i > 0 && (inhistory_type[i - 1] == QALCULATE_HISTORY_REGISTER_MOVED || inhistory_type[i - 1] == QALCULATE_HISTORY_RPN_OPERATION)) {
					gtk_list_store_set(historystore, &history_iter, 1, i, -1);
				}
				break;
			}
			case QALCULATE_HISTORY_PARSE: {
				gchar *expr_str = NULL;
				gtk_tree_model_get(GTK_TREE_MODEL(historystore), &history_iter, 0, &expr_str, -1);
				history_str = expr_str;
				g_free(expr_str);
				history_str += "<span font-style=\"italic\" foreground=\"";
				history_str += history_parse_color;
				history_str += "\">  = ";
				history_str += fix_history_string(inhistory[i]);
				history_str += "</span>";
				gtk_list_store_set(historystore, &history_iter, 0, history_str.c_str(), -1);
				if(i > 0 && (inhistory_type[i - 1] == QALCULATE_HISTORY_REGISTER_MOVED || inhistory_type[i - 1] == QALCULATE_HISTORY_RPN_OPERATION)) {
					gtk_list_store_set(historystore, &history_iter, 1, i, -1);
				}
				break;
			}
			case QALCULATE_HISTORY_PARSE_APPROXIMATE: {
				string str = "  ";
				if(printops.use_unicode_signs && can_display_unicode_string_function(SIGN_ALMOST_EQUAL, (void*) historyview)) {
					str += SIGN_ALMOST_EQUAL " ";
				} else {
					str += "= ";
					str += _("approx.");
					str += " ";
				}
				gchar *expr_str = NULL;
				gtk_tree_model_get(GTK_TREE_MODEL(historystore), &history_iter, 0, &expr_str, -1);
				history_str = expr_str;
				g_free(expr_str);
				history_str += "<span font-style=\"italic\" foreground=\"";
				history_str += history_parse_color;
				history_str += "\">";
				history_str += str;
				history_str += fix_history_string(inhistory[i]);
				history_str += "</span>";
				gtk_list_store_set(historystore, &history_iter, 0, history_str.c_str(), -1);
				if(i > 0 && (inhistory_type[i - 1] == QALCULATE_HISTORY_REGISTER_MOVED || inhistory_type[i - 1] == QALCULATE_HISTORY_RPN_OPERATION)) {
					gtk_list_store_set(historystore, &history_iter, 1, i, -1);
				}
				break;
			}
			case QALCULATE_HISTORY_WARNING: {
				history_str = "<span foreground=\"";
				history_str += history_warning_color;
				history_str += "\">- ";
				history_str += fix_history_string(inhistory[i]);
				history_str += "</span>";
				gtk_list_store_insert_with_values(historystore, &history_iter, -1, 0, history_str.c_str(), 1, i, 4, 0, -1);
				break;
			}
			case QALCULATE_HISTORY_ERROR: {
				history_str = "<span foreground=\"";
				history_str += history_error_color;
				history_str += "\">- ";
				history_str += fix_history_string(inhistory[i]);
				history_str += "</span>";
				gtk_list_store_insert_with_values(historystore, &history_iter, -1, 0, history_str.c_str(), 1, i, 4, 0, -1);
				break;
			}
			case QALCULATE_HISTORY_OLD: {
				gtk_list_store_insert_with_values(historystore, &history_iter, -1, 0, inhistory[i].c_str(), 1, i, 4, 0, -1);
				break;
			}
		}
	}	

	stackstore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(stackview), GTK_TREE_MODEL(stackstore));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(stackview));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	renderer = gtk_cell_renderer_text_new();
	g_object_set (G_OBJECT(renderer), "xalign", 0.5, NULL);
	column = gtk_tree_view_column_new_with_attributes(_("Index"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(stackview), column);
	register_renderer = gtk_cell_renderer_text_new();
	//g_object_set (G_OBJECT(register_renderer), "editable", FALSE, "ellipsize", PANGO_ELLIPSIZE_END, "xalign", 1.0, NULL);
	g_object_set(G_OBJECT(register_renderer), "editable", TRUE, "ellipsize", PANGO_ELLIPSIZE_END, "xalign", 1.0, "mode", GTK_CELL_RENDERER_MODE_EDITABLE, NULL);
	g_signal_connect((gpointer) register_renderer, "edited", G_CALLBACK(on_stackview_item_edited), NULL);
	g_signal_connect((gpointer) register_renderer, "editing-started", G_CALLBACK(on_stackview_item_editing_started), NULL);
	g_signal_connect((gpointer) register_renderer, "editing-canceled", G_CALLBACK(on_stackview_item_editing_canceled), NULL);
	register_column = gtk_tree_view_column_new_with_attributes(_("Value"), register_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(stackview), register_column);
	g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_stackview_selection_changed), NULL);

	if(rpn_mode) {
		gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(main_builder, "label_equals")), _("Ent"));
		gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(main_builder, "expression_button_equals")), _("Ent"));
		gtk_widget_set_tooltip_text(GTK_WIDGET(gtk_builder_get_object(main_builder, "button_equals")), _("Calculate expression and add to stack"));
		gtk_widget_set_tooltip_text(GTK_WIDGET(gtk_builder_get_object(main_builder, "expression_button")), _("Calculate expression and add to stack"));
	} else {
		gtk_widget_hide(expander_stack);
	}

	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_item_save_image")), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(main_builder, "popup_menu_item_save_image")), FALSE);

/*	Completion	*/
	completion_view = GTK_WIDGET(gtk_builder_get_object(main_builder, "completionview"));
	completion_scrolled = GTK_WIDGET(gtk_builder_get_object(main_builder, "completionscrolled"));
	gtk_widget_set_size_request(gtk_scrolled_window_get_vscrollbar(GTK_SCROLLED_WINDOW(completion_scrolled)), -1, 0);
	completion_window = GTK_WIDGET(gtk_builder_get_object(main_builder, "completionwindow"));
	completion_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_BOOLEAN);
	completion_filter = gtk_tree_model_filter_new(GTK_TREE_MODEL(completion_store), NULL);
	gtk_tree_model_filter_set_visible_column(GTK_TREE_MODEL_FILTER(completion_filter), 3);
	gtk_tree_view_set_model(GTK_TREE_VIEW(completion_view), completion_filter);

	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	renderer = gtk_cell_renderer_text_new();
	GtkCellArea *area = gtk_cell_area_box_new();
	gtk_cell_area_box_set_spacing(GTK_CELL_AREA_BOX(area), 12);
	column = gtk_tree_view_column_new_with_area(area);
	gtk_cell_area_box_pack_start(GTK_CELL_AREA_BOX(area), renderer, TRUE, TRUE, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(area), renderer, "markup", 0, NULL);
	renderer = gtk_cell_renderer_text_new();
	g_object_set(G_OBJECT(renderer), "style", PANGO_STYLE_ITALIC, NULL);
	gtk_cell_area_box_pack_end(GTK_CELL_AREA_BOX(area), renderer, FALSE, TRUE, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(area), renderer, "text", 1, NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gtk_tree_view_append_column(GTK_TREE_VIEW(completion_view), column);
	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(completion_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(completion_store), 0, GTK_SORT_ASCENDING);
	
	for(size_t i = 0; i < modes.size(); i++) {
		GtkWidget *item = gtk_menu_item_new_with_label(modes[i].name.c_str()); 
		gtk_widget_show(item); 
		g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(on_menu_item_meta_mode_activate), (gpointer) modes[i].name.c_str());
		gtk_menu_shell_insert(GTK_MENU_SHELL(gtk_builder_get_object(main_builder, "menu_meta_modes")), item, (gint) i);
		mode_items.push_back(item);
		item = gtk_menu_item_new_with_label(modes[i].name.c_str()); 
		gtk_widget_show(item); 
		g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(on_menu_item_meta_mode_activate), (gpointer) modes[i].name.c_str());
		gtk_menu_shell_insert(GTK_MENU_SHELL(gtk_builder_get_object(main_builder, "menu_result_popup_meta_modes")), item, (gint) i);
		popup_result_mode_items.push_back(item);
	}
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_item_meta_mode_delete")), modes.size() > 2);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(main_builder, "menu_item_result_popup_meta_mode_delete")), modes.size() > 2);
	
	tUnitSelectorCategories = GTK_WIDGET(gtk_builder_get_object(main_builder, "convert_treeview_category"));
	tUnitSelector = GTK_WIDGET(gtk_builder_get_object(main_builder, "convert_treeview_unit"));
	
	tUnitSelector_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tUnitSelector_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tUnitSelector_store), 0, GTK_SORT_ASCENDING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tUnitSelector), GTK_TREE_MODEL(tUnitSelector_store));
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tUnitSelector));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tUnitSelector), column);
	g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tUnitSelector_selection_changed), NULL);
	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(tUnitSelector), TRUE);

	tUnitSelectorCategories_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tUnitSelectorCategories), GTK_TREE_MODEL(tUnitSelectorCategories_store));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tUnitSelectorCategories));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Category"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tUnitSelectorCategories), column);
	g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tUnitSelectorCategories_selection_changed), NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tUnitSelectorCategories_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tUnitSelectorCategories_store), 0, GTK_SORT_ASCENDING);
	
	if(win_width > 0) {
		gtk_window_resize(GTK_WINDOW(gtk_builder_get_object(main_builder, "main_window")), win_width, win_height);
	}

	gtk_widget_show (GTK_WIDGET(gtk_builder_get_object(main_builder, "main_window")));
	
	set_result_size_request();
	set_expression_size_request();
	
	gtk_window_resize(GTK_WINDOW(gtk_builder_get_object(main_builder, "main_window")), win_width > 0 ? win_width : 100, win_height > 0 ? win_height : 100);

	update_status_text();
	
}

GtkWidget*
get_functions_dialog (void)
{

	if(!functions_builder) {
	
		functions_builder =  getBuilder("functions.ui");
		g_assert(functions_builder != NULL);
	
		g_assert (gtk_builder_get_object(functions_builder, "functions_dialog") != NULL);
	
		tFunctionCategories = GTK_WIDGET(gtk_builder_get_object(functions_builder, "functions_treeview_category"));
		tFunctions	= GTK_WIDGET(gtk_builder_get_object(functions_builder, "functions_treeview_function"));

		tFunctions_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tFunctions), GTK_TREE_MODEL(tFunctions_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tFunctions));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Function"), renderer, "text", 0, NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tFunctions), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tFunctions_selection_changed), NULL);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tFunctions_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tFunctions_store), 0, GTK_SORT_ASCENDING);

		tFunctionCategories_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tFunctionCategories), GTK_TREE_MODEL(tFunctionCategories_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tFunctionCategories));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Category"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tFunctionCategories), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tFunctionCategories_selection_changed), NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tFunctionCategories_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tFunctionCategories_store), 0, GTK_SORT_ASCENDING);

		GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(functions_builder, "functions_textview_description")));
		gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
		gtk_text_buffer_create_tag(buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);
		
		if(functions_width > 0 && functions_height > 0) gtk_window_resize(GTK_WINDOW(gtk_builder_get_object(functions_builder, "functions_dialog")), functions_width, functions_height);
		if(functions_hposition > 0) gtk_paned_set_position(GTK_PANED(gtk_builder_get_object(functions_builder, "functions_hpaned")), functions_hposition);
		if(functions_vposition > 0) gtk_paned_set_position(GTK_PANED(gtk_builder_get_object(functions_builder, "functions_vpaned")), functions_vposition);

		gtk_builder_connect_signals(functions_builder, NULL);

		update_functions_tree();
		
	}

	return GTK_WIDGET(gtk_builder_get_object(functions_builder, "functions_dialog"));
}

GtkWidget*
get_variables_dialog (void)
{
	if(!variables_builder) {

		variables_builder = getBuilder("variables.ui");
		g_assert(variables_builder != NULL);
	
		g_assert (gtk_builder_get_object(variables_builder, "variables_dialog") != NULL);

		tVariableCategories = GTK_WIDGET(gtk_builder_get_object(variables_builder, "variables_treeview_category"));
		tVariables = GTK_WIDGET(gtk_builder_get_object(variables_builder, "variables_treeview_variable"));

		tVariables_store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tVariables), GTK_TREE_MODEL(tVariables_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tVariables));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Variable"), renderer, "text", 0, NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tVariables), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Value"), renderer, "text", 1, NULL);
		//g_object_set(G_OBJECT(renderer), "ellipsize", PANGO_ELLIPSIZE_NONE, NULL);
		gtk_tree_view_column_set_sort_column_id(column, 1);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tVariables), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tVariables_selection_changed), NULL);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tVariables_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tVariables_store), 1, int_string_sort_func, GINT_TO_POINTER(1), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tVariables_store), 0, GTK_SORT_ASCENDING);

		gtk_tree_view_set_enable_search(GTK_TREE_VIEW(tVariables), TRUE);

		tVariableCategories_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tVariableCategories), GTK_TREE_MODEL(tVariableCategories_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tVariableCategories));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Category"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tVariableCategories), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tVariableCategories_selection_changed), NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tVariableCategories_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tVariableCategories_store), 0, GTK_SORT_ASCENDING);
		
		if(variables_width > 0 && variables_height > 0) gtk_window_resize(GTK_WINDOW(gtk_builder_get_object(variables_builder, "variables_dialog")), variables_width, variables_height);
		if(variables_position > 0) gtk_paned_set_position(GTK_PANED(gtk_builder_get_object(variables_builder, "variables_hpaned")), variables_position);

		gtk_builder_connect_signals(variables_builder, NULL);

		update_variables_tree();

	}
	
	return GTK_WIDGET(gtk_builder_get_object(variables_builder, "variables_dialog"));
}

GtkWidget*
get_units_dialog (void)
{

	if(!units_builder) {
	
		units_builder = getBuilder("units.ui");
		g_assert(units_builder != NULL);
	
		g_assert (gtk_builder_get_object(units_builder, "units_dialog") != NULL);
	
		tUnitCategories = GTK_WIDGET(gtk_builder_get_object(units_builder, "units_treeview_category"));
		tUnits		= GTK_WIDGET(gtk_builder_get_object(units_builder, "units_treeview_unit"));

		tUnits_store = gtk_list_store_new(UNITS_N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tUnits), GTK_TREE_MODEL(tUnits_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tUnits));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", UNITS_TITLE_COLUMN, NULL);
		gtk_tree_view_column_set_sort_column_id(column, UNITS_TITLE_COLUMN);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tUnits), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Unit"), renderer, "text", UNITS_NAMES_COLUMN, NULL);
		gtk_tree_view_column_set_sort_column_id(column, UNITS_NAMES_COLUMN);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tUnits), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Unit"), renderer, "text", UNITS_BASE_COLUMN, NULL);
		gtk_tree_view_column_set_sort_column_id(column, UNITS_BASE_COLUMN);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tUnits), column);	
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tUnits_selection_changed), NULL);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tUnits_store), UNITS_TITLE_COLUMN, string_sort_func, GINT_TO_POINTER(UNITS_TITLE_COLUMN), NULL);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tUnits_store), UNITS_NAMES_COLUMN, string_sort_func, GINT_TO_POINTER(UNITS_NAMES_COLUMN), NULL);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tUnits_store), UNITS_BASE_COLUMN, string_sort_func, GINT_TO_POINTER(UNITS_BASE_COLUMN), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tUnits_store), UNITS_TITLE_COLUMN, GTK_SORT_ASCENDING);

		gtk_tree_view_set_enable_search(GTK_TREE_VIEW(tUnits), TRUE);

		tUnitCategories_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tUnitCategories), GTK_TREE_MODEL(tUnitCategories_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tUnitCategories));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Category"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tUnitCategories), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tUnitCategories_selection_changed), NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tUnitCategories_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tUnitCategories_store), 0, GTK_SORT_ASCENDING);
		
		if(units_width > 0 && units_height > 0) gtk_window_resize(GTK_WINDOW(gtk_builder_get_object(units_builder, "units_dialog")), units_width, units_height);
		if(units_position > 0) gtk_paned_set_position(GTK_PANED(gtk_builder_get_object(units_builder, "units_hpaned")), units_position);
		
		gtk_builder_connect_signals(units_builder, NULL);

		update_units_tree();
	
		gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object(units_builder, "units_entry_from_val")), "1");	
		gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object(units_builder, "units_entry_to_val")), "1");	
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(units_builder, "units_entry_from_val")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(units_builder, "units_entry_to_val")), 1.0);
		
	}
	
	return GTK_WIDGET(gtk_builder_get_object(units_builder, "units_dialog"));
}

GtkWidget*
get_datasets_dialog (void)
{

	if(!datasets_builder) {
	
		datasets_builder = getBuilder("datasets.ui");
		g_assert(datasets_builder != NULL);
	
		g_assert (gtk_builder_get_object(datasets_builder, "datasets_dialog") != NULL);
	
		tDatasets = GTK_WIDGET(gtk_builder_get_object(datasets_builder, "datasets_treeview_datasets"));
		tDataObjects = GTK_WIDGET(gtk_builder_get_object(datasets_builder, "datasets_treeview_objects"));

		tDataObjects_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tDataObjects), GTK_TREE_MODEL(tDataObjects_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tDataObjects));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("Key 1", renderer, "text", 0, NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tDataObjects), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("Key 2", renderer, "text", 1, NULL);
		gtk_tree_view_column_set_sort_column_id(column, 1);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tDataObjects), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("Key 3", renderer, "text", 2, NULL);
		gtk_tree_view_column_set_sort_column_id(column, 2);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tDataObjects), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tDataObjects_selection_changed), NULL);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tDataObjects_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tDataObjects_store), 0, GTK_SORT_ASCENDING);

		tDatasets_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tDatasets), GTK_TREE_MODEL(tDatasets_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tDatasets));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Data Set"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tDatasets), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tDatasets_selection_changed), NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tDatasets_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tDatasets_store), 0, GTK_SORT_ASCENDING);

		GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(datasets_builder, "datasets_textview_description")));
		gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
		gtk_text_buffer_create_tag(buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);

		if(datasets_width > 0 && datasets_height > 0) gtk_window_resize(GTK_WINDOW(gtk_builder_get_object(datasets_builder, "datasets_dialog")), datasets_width, datasets_height);
		if(datasets_hposition > 0) gtk_paned_set_position(GTK_PANED(gtk_builder_get_object(datasets_builder, "datasets_hpaned")), datasets_hposition);
		if(datasets_vposition1 > 0) gtk_paned_set_position(GTK_PANED(gtk_builder_get_object(datasets_builder, "datasets_vpaned1")), datasets_vposition1);
		if(datasets_vposition2 > 0) gtk_paned_set_position(GTK_PANED(gtk_builder_get_object(datasets_builder, "datasets_vpaned2")), datasets_vposition2);
		
		gtk_builder_connect_signals(datasets_builder, NULL);

		update_datasets_tree();

	}

	return GTK_WIDGET(gtk_builder_get_object(datasets_builder, "datasets_dialog"));
}


GtkWidget*
get_preferences_dialog (void)
{
	if(!preferences_builder) {
	
		preferences_builder = getBuilder("preferences.ui");
		g_assert(preferences_builder != NULL);
	
		g_assert (gtk_builder_get_object(preferences_builder, "preferences_dialog") != NULL);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_display_expression_status")), display_expression_status);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_fetch_exchange_rates")), fetch_exchange_rates_at_startup);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_save_mode")), save_mode_on_exit);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_allow_multiple_instances")), allow_multiple_instances);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_unicode_signs")), printops.use_unicode_signs);	
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_lower_case_numbers")), printops.lower_case_numbers);	
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_lower_case_e")), printops.lower_case_e);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_alternative_base_prefixes")), printops.base_display == BASE_DISPLAY_ALTERNATIVE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_spell_out_logical_operators")), printops.spell_out_logical_operators);	
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_save_defs")), save_defs_on_exit);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_rpn_keypad_only")), rpn_keypad_only);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_dot_as_separator")), evalops.parse_options.dot_as_separator);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_comma_as_separator")), evalops.parse_options.comma_as_separator);
		if(CALCULATOR->getDecimalPoint() == ".") gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_dot_as_separator")));
		if(CALCULATOR->getDecimalPoint() == ",") gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_comma_as_separator")));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_custom_result_font")), use_custom_result_font);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_custom_expression_font")), use_custom_expression_font);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_checkbutton_custom_status_font")), use_custom_status_font);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_button_result_font")), use_custom_result_font);	
		gtk_font_button_set_font_name(GTK_FONT_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_button_result_font")), custom_result_font.c_str());
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_button_expression_font")), use_custom_expression_font);	
		gtk_font_button_set_font_name(GTK_FONT_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_button_expression_font")), custom_expression_font.c_str());
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_button_status_font")), use_custom_status_font);	
		gtk_font_button_set_font_name(GTK_FONT_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_button_status_font")), custom_status_font.c_str());
		GdkRGBA c;
		gdk_rgba_parse(&c, status_error_color.c_str());
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(gtk_builder_get_object(preferences_builder, "colorbutton_status_error_color")), &c);
		gdk_rgba_parse(&c, status_warning_color.c_str());
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(gtk_builder_get_object(preferences_builder, "colorbutton_status_warning_color")), &c);
		if(can_display_unicode_string_function(SIGN_MULTIDOT, (void*) gtk_builder_get_object(preferences_builder, "preferences_radiobutton_dot"))) gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_dot")), SIGN_MULTIDOT);
		else gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_dot")), SIGN_SMALLCIRCLE);
		gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_ex")), SIGN_MULTIPLICATION);
		switch(printops.multiplication_sign) {
			case MULTIPLICATION_SIGN_DOT: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_dot")), TRUE);
				break;
			}
			case MULTIPLICATION_SIGN_X: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_ex")), TRUE);
				break;
			}
			default: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_asterisk")), TRUE);
				break;
			}
		}
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_asterisk")), printops.use_unicode_signs);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_ex")), printops.use_unicode_signs);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_dot")), printops.use_unicode_signs);
		gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_division_slash")), " " SIGN_DIVISION_SLASH " ");
		gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_division")), SIGN_DIVISION);
		switch(printops.division_sign) {
			case DIVISION_SIGN_DIVISION_SLASH: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_division_slash")), TRUE);
				break;
			}
			case DIVISION_SIGN_DIVISION: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_division")), TRUE);
				break;
			}
			default: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_slash")), TRUE);
				break;
			}
		}
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_slash")), printops.use_unicode_signs);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_division_slash")), printops.use_unicode_signs);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_radiobutton_division")), printops.use_unicode_signs);
		gtk_builder_connect_signals(preferences_builder, NULL);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(preferences_builder, "preferences_update_exchange_rates_spin_button")), (double) auto_update_exchange_rates);
		
	}

	return GTK_WIDGET(gtk_builder_get_object(preferences_builder, "preferences_dialog"));
}

GtkWidget*
get_unit_edit_dialog (void)
{

	if(!unitedit_builder) {
	
		unitedit_builder = getBuilder("unitedit.ui");
		g_assert(unitedit_builder != NULL);
	
		g_assert (gtk_builder_get_object(unitedit_builder, "unit_edit_dialog") != NULL);
		
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(unitedit_builder, "unit_edit_combobox_class")), 0);
		
		gtk_builder_connect_signals(unitedit_builder, NULL);
	
	}
	
	/* populate combo menu */
	
	GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
	GList *items = NULL;
	for(size_t i = 0; i < CALCULATOR->units.size(); i++) {
		if(!CALCULATOR->units[i]->category().empty()) {
			//add category if not present
			if(g_hash_table_lookup(hash, (gconstpointer) CALCULATOR->units[i]->category().c_str()) == NULL) {
				items = g_list_insert_sorted(items, (gpointer) CALCULATOR->units[i]->category().c_str(), (GCompareFunc) compare_categories);
				//remember added categories
				g_hash_table_insert(hash, (gpointer) CALCULATOR->units[i]->category().c_str(), (gpointer) hash);
			}
		}
	}
	for(GList *l = items; l != NULL; l = l->next) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gtk_builder_get_object(unitedit_builder, "unit_edit_combo_category")), (const gchar*) l->data);
	}
	g_hash_table_destroy(hash);	
	g_list_free(items);

	return GTK_WIDGET(gtk_builder_get_object(unitedit_builder, "unit_edit_dialog"));
}

GtkWidget*
get_function_edit_dialog (void)
{

	if(!functionedit_builder) {
	
		functionedit_builder = getBuilder("functionedit.ui");
		g_assert(functionedit_builder != NULL);
	
		g_assert (gtk_builder_get_object(functionedit_builder, "function_edit_dialog") != NULL);
		
		tFunctionArguments = GTK_WIDGET(gtk_builder_get_object(functionedit_builder, "function_edit_treeview_arguments"));
		tFunctionArguments_store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tFunctionArguments), GTK_TREE_MODEL(tFunctionArguments_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tFunctionArguments));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tFunctionArguments), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Type"), renderer, "text", 1, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tFunctionArguments), column);	
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tFunctionArguments_selection_changed), NULL);
		
		tSubfunctions = GTK_WIDGET(gtk_builder_get_object(functionedit_builder, "function_edit_treeview_subfunctions"));
		tSubfunctions_store = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_BOOLEAN);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tSubfunctions), GTK_TREE_MODEL(tSubfunctions_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tSubfunctions));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Reference"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tSubfunctions), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Expression"), renderer, "text", 1, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tSubfunctions), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Precalculate"), renderer, "text", 2, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tSubfunctions), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tSubfunctions_selection_changed), NULL);
		
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(functionedit_builder, "function_edit_combobox_argument_type")), 0);
		
		gtk_builder_connect_signals(functionedit_builder, NULL);
	
	}
	
	/* populate combo menu */
	
	GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
	GList *items = NULL;
	for(size_t i = 0; i < CALCULATOR->functions.size(); i++) {
		if(!CALCULATOR->functions[i]->category().empty()) {
			//add category if not present
			if(g_hash_table_lookup(hash, (gconstpointer) CALCULATOR->functions[i]->category().c_str()) == NULL) {
				items = g_list_insert_sorted(items, (gpointer) CALCULATOR->functions[i]->category().c_str(), (GCompareFunc) compare_categories);
				//remember added categories
				g_hash_table_insert(hash, (gpointer) CALCULATOR->functions[i]->category().c_str(), (gpointer) hash);
			}
		}
	}
	for(GList *l = items; l != NULL; l = l->next) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gtk_builder_get_object(functionedit_builder, "function_edit_combo_category")), (const gchar*) l->data);
	}
	g_hash_table_destroy(hash);	
	g_list_free(items);
	
	return GTK_WIDGET(gtk_builder_get_object(functionedit_builder, "function_edit_dialog"));
}

GtkWidget*
get_simple_function_edit_dialog (void)
{

	if(!simplefunctionedit_builder) {
	
		simplefunctionedit_builder = getBuilder("simplefunctionedit.ui");
		g_assert(simplefunctionedit_builder != NULL);
	
		g_assert(gtk_builder_get_object(simplefunctionedit_builder, "simple_function_edit_dialog") != NULL);
		
		gtk_builder_connect_signals(simplefunctionedit_builder, NULL);
	
	}
	
	return GTK_WIDGET(gtk_builder_get_object(simplefunctionedit_builder, "simple_function_edit_dialog"));
}

GtkWidget*
get_variable_edit_dialog (void)
{
	
	if(!variableedit_builder) {
	
		variableedit_builder = getBuilder("variableedit.ui");
		g_assert(variableedit_builder != NULL);
			
		g_assert (gtk_builder_get_object(variableedit_builder, "variable_edit_dialog") != NULL);
		
		gtk_builder_connect_signals(variableedit_builder, NULL);
	
	}
	/* populate combo menu */
	
	GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
	GList *items = NULL;
	for(size_t i = 0; i < CALCULATOR->variables.size(); i++) {
		if(!CALCULATOR->variables[i]->category().empty()) {
			//add category if not present
			if(g_hash_table_lookup(hash, (gconstpointer) CALCULATOR->variables[i]->category().c_str()) == NULL) {
				items = g_list_insert_sorted(items, (gpointer) CALCULATOR->variables[i]->category().c_str(), (GCompareFunc) compare_categories);
				//remember added categories
				g_hash_table_insert(hash, (gpointer) CALCULATOR->variables[i]->category().c_str(), (gpointer) hash);
			}
		}
	}
	for(GList *l = items; l != NULL; l = l->next) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gtk_builder_get_object(variableedit_builder, "variable_edit_combo_category")), (const gchar*) l->data);
	}
	g_hash_table_destroy(hash);
	g_list_free(items);

	return GTK_WIDGET(gtk_builder_get_object(variableedit_builder, "variable_edit_dialog"));
}

GtkWidget*
get_unknown_edit_dialog (void)
{
	
	if(!unknownedit_builder) {
	
		unknownedit_builder = getBuilder("unknownedit.ui");
		g_assert(unknownedit_builder != NULL);
	
		g_assert (gtk_builder_get_object(unknownedit_builder, "unknown_edit_dialog") != NULL);

		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(unknownedit_builder, "unknown_edit_combobox_type")), 0);
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(unknownedit_builder, "unknown_edit_combobox_sign")), 0);
		
		gtk_builder_connect_signals(unknownedit_builder, NULL);
	
	}
	
	/* populate combo menu */
	
	GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
	GList *items = NULL;
	for(size_t i = 0; i < CALCULATOR->variables.size(); i++) {
		if(!CALCULATOR->variables[i]->category().empty()) {
			//add category if not present
			if(g_hash_table_lookup(hash, (gconstpointer) CALCULATOR->variables[i]->category().c_str()) == NULL) {
				items = g_list_insert_sorted(items, (gpointer) CALCULATOR->variables[i]->category().c_str(), (GCompareFunc) compare_categories);
				//remember added categories
				g_hash_table_insert(hash, (gpointer) CALCULATOR->variables[i]->category().c_str(), (gpointer) hash);
			}
		}
	}
	for(GList *l = items; l != NULL; l = l->next) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gtk_builder_get_object(unknownedit_builder, "unknown_edit_combo_category")), (const gchar*) l->data);
	}
	g_hash_table_destroy(hash);
	g_list_free(items);

	return GTK_WIDGET(gtk_builder_get_object(unknownedit_builder, "unknown_edit_dialog"));
}

GtkWidget*
get_matrix_edit_dialog (void)
{
	if(!matrixedit_builder) {
	
		matrixedit_builder = getBuilder("matrixedit.ui");
		g_assert(matrixedit_builder != NULL);
	
		g_assert (gtk_builder_get_object(matrixedit_builder, "matrix_edit_dialog") != NULL);
		
		//GType types[20000];
		GType types[10000];
		//for(gint i = 0; i < 20000; i += 2) {
		for(gint i = 0; i < 10000; i += 1) {
			types[i] = G_TYPE_STRING;
			//types[i + 1] = GDK_TYPE_COLOR;
		}
		tMatrixEdit_store = gtk_list_store_newv(10000, types);
		tMatrixEdit = GTK_WIDGET(gtk_builder_get_object(matrixedit_builder, "matrix_edit_view"));
		gtk_tree_view_set_model (GTK_TREE_VIEW(tMatrixEdit), GTK_TREE_MODEL(tMatrixEdit_store));
		//gtk_widget_override_background_color(tMatrixEdit, GTK_STATE_NORMAL, &stackview->style->bg[GTK_STATE_NORMAL]);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tMatrixEdit));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_NONE);
		
		gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(gtk_builder_get_object(matrixedit_builder, "matrix_edit_combo_category")))), _("Matrices"));
		
		gtk_builder_connect_signals(matrixedit_builder, NULL);
		
	}

	/* populate combo menu */
	
	GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
	GList *items = NULL;
	for(size_t i = 0; i < CALCULATOR->variables.size(); i++) {
		if(!CALCULATOR->variables[i]->category().empty()) {
			//add category if not present
			if(g_hash_table_lookup(hash, (gconstpointer) CALCULATOR->variables[i]->category().c_str()) == NULL) {
				items = g_list_insert_sorted(items, (gpointer) CALCULATOR->variables[i]->category().c_str(), (GCompareFunc) compare_categories);
				//remember added categories
				g_hash_table_insert(hash, (gpointer) CALCULATOR->variables[i]->category().c_str(), (gpointer) hash);
			}
		}
	}
	for(GList *l = items; l != NULL; l = l->next) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gtk_builder_get_object(matrixedit_builder, "matrix_edit_combo_category")), (const gchar*) l->data);
	}
	g_hash_table_destroy(hash);	
	g_list_free(items);


	return GTK_WIDGET(gtk_builder_get_object(matrixedit_builder, "matrix_edit_dialog"));
}
GtkWidget*
get_matrix_dialog (void)
{
	if(!matrix_builder) {
	
		matrix_builder = getBuilder("matrix.ui");
		g_assert(matrix_builder != NULL);
	
		g_assert (gtk_builder_get_object(matrix_builder, "matrix_dialog") != NULL);

		/*GType types[20000];
		for(gint i = 0; i < 20000; i += 2) {
			types[i] = G_TYPE_STRING;
			types[i + 1] = GDK_TYPE_COLOR;
		}*/
		GType types[10000];
		for(gint i = 0; i < 10000; i++) {
			types[i] = G_TYPE_STRING;
		}
		tMatrix_store = gtk_list_store_newv(10000, types);
		tMatrix = GTK_WIDGET(gtk_builder_get_object(matrix_builder, "matrix_view"));
		gtk_tree_view_set_model (GTK_TREE_VIEW(tMatrix), GTK_TREE_MODEL(tMatrix_store));
		//gtk_widget_modify_base(tMatrix, GTK_STATE_NORMAL, &stackview->style->bg[GTK_STATE_NORMAL]);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tMatrix));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_NONE);

		gtk_builder_connect_signals(matrix_builder, NULL);

	}

	return GTK_WIDGET(gtk_builder_get_object(matrix_builder, "matrix_dialog"));
	
}

GtkWidget*
get_dataobject_edit_dialog (void)
{

	return GTK_WIDGET(gtk_builder_get_object(datasets_builder, "dataobject_edit_dialog"));
}

GtkWidget*
get_dataset_edit_dialog (void)
{

	if(!datasetedit_builder) {

		datasetedit_builder = getBuilder("datasetedit.ui");
		g_assert(datasetedit_builder != NULL);

		g_assert (gtk_builder_get_object(datasetedit_builder, "dataset_edit_dialog") != NULL);
		
		tDataProperties = GTK_WIDGET(gtk_builder_get_object(datasetedit_builder, "dataset_edit_treeview_properties"));
		tDataProperties_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tDataProperties), GTK_TREE_MODEL(tDataProperties_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tDataProperties));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Title"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tDataProperties), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", 1, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tDataProperties), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Type"), renderer, "text", 2, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tDataProperties), column);	
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tDataProperties_selection_changed), NULL);		
		
		gtk_builder_connect_signals(datasetedit_builder, NULL);
	
	}

	return GTK_WIDGET(gtk_builder_get_object(datasetedit_builder, "dataset_edit_dialog"));
}

GtkWidget*
get_dataproperty_edit_dialog (void)
{

	if(!datasetedit_builder) {

		datasetedit_builder = getBuilder("datasetedit.ui");
		g_assert(datasetedit_builder != NULL);

		g_assert (gtk_builder_get_object(datasetedit_builder, "dataproperty_edit_dialog") != NULL);

		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(datasetedit_builder, "dataproperty_edit_combobox_type")), 0);
		
		gtk_builder_connect_signals(datasetedit_builder, NULL);
	
	}

	return GTK_WIDGET(gtk_builder_get_object(datasetedit_builder, "dataproperty_edit_dialog"));
}


GtkWidget* 
get_names_edit_dialog (void)
{
	if(!namesedit_builder) {
	
		namesedit_builder = getBuilder("namesedit.ui");
		g_assert(namesedit_builder != NULL);
	
		g_assert (gtk_builder_get_object(namesedit_builder, "names_edit_dialog") != NULL);
		
		tNames = GTK_WIDGET(gtk_builder_get_object(namesedit_builder, "names_edit_treeview"));

		tNames_store = gtk_list_store_new(NAMES_N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tNames), GTK_TREE_MODEL(tNames_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tNames));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", NAMES_NAME_COLUMN, NULL);
		gtk_tree_view_column_set_sort_column_id(column, NAMES_NAME_COLUMN);
		gtk_tree_view_column_set_expand(column, TRUE);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tNames), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Abbreviation"), renderer, "text", NAMES_ABBREVIATION_STRING_COLUMN, NULL);
		gtk_tree_view_column_set_sort_column_id(column, NAMES_ABBREVIATION_STRING_COLUMN);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tNames), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Plural"), renderer, "text", NAMES_PLURAL_STRING_COLUMN, NULL);
		gtk_tree_view_column_set_sort_column_id(column, NAMES_PLURAL_STRING_COLUMN);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tNames), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Reference"), renderer, "text", NAMES_REFERENCE_STRING_COLUMN, NULL);
		gtk_tree_view_column_set_sort_column_id(column, NAMES_REFERENCE_STRING_COLUMN);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tNames), column);	
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tNames_selection_changed), NULL);
		
		gtk_builder_connect_signals(namesedit_builder, NULL);

	}

	return GTK_WIDGET(gtk_builder_get_object(namesedit_builder, "names_edit_dialog"));
}

GtkWidget*
get_csv_import_dialog (void)
{

	if(!csvimport_builder) {
	
		csvimport_builder = getBuilder("csvimport.ui");
		g_assert(csvimport_builder != NULL);
	
		g_assert (gtk_builder_get_object(csvimport_builder, "csv_import_dialog") != NULL);
		
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(csvimport_builder, "csv_import_combobox_delimiter")), 0);
		
		gtk_builder_connect_signals(csvimport_builder, NULL);
	
	}
	/* populate combo menu */
	
	GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
	GList *items = NULL;
	for(size_t i = 0; i < CALCULATOR->variables.size(); i++) {
		if(!CALCULATOR->variables[i]->category().empty()) {
			//add category if not present
			if(g_hash_table_lookup(hash, (gconstpointer) CALCULATOR->variables[i]->category().c_str()) == NULL) {
				items = g_list_append(items, (gpointer) CALCULATOR->variables[i]->category().c_str());
				//remember added categories
				g_hash_table_insert(hash, (gpointer) CALCULATOR->variables[i]->category().c_str(), (gpointer) hash);
			}
		}
	}
	for(GList *l = items; l != NULL; l = l->next) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gtk_builder_get_object(csvimport_builder, "csv_import_combo_category")), (const gchar*) l->data);
	}
	g_hash_table_destroy(hash);	
	g_list_free(items);

	return GTK_WIDGET(gtk_builder_get_object(csvimport_builder, "csv_import_dialog"));
}

GtkWidget*
get_csv_export_dialog (void)
{

	if(!csvexport_builder) {
	
		csvexport_builder = getBuilder("csvexport.ui");
		g_assert(csvexport_builder != NULL);
	
		g_assert (gtk_builder_get_object(csvexport_builder, "csv_export_dialog") != NULL);
		
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(csvexport_builder, "csv_export_combobox_delimiter")), 0);
		
		gtk_builder_connect_signals(csvexport_builder, NULL);
	
	}
	
	return GTK_WIDGET(gtk_builder_get_object(csvexport_builder, "csv_export_dialog"));
	
}

GtkWidget* get_set_base_dialog (void) {
	if(!setbase_builder) {
	
		setbase_builder = getBuilder("setbase.ui");
		g_assert(setbase_builder != NULL);
	
		g_assert (gtk_builder_get_object(setbase_builder, "set_base_dialog") != NULL);

		switch(evalops.parse_options.base) {
			case BASE_BINARY: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_binary")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_input_other")), FALSE);
				break;
			}
			case BASE_OCTAL: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_octal")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_input_other")), FALSE);
				break;
			}
			case BASE_DECIMAL: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_decimal")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_input_other")), FALSE);
				break;
			}
			case BASE_HEXADECIMAL: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_hexadecimal")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_input_other")), FALSE);
				break;
			}
			case BASE_ROMAN_NUMERALS: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_roman")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_input_other")), FALSE);
				break;
			}
			default: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_input_other")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_input_other")), TRUE);
				gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_input_other")), evalops.parse_options.base);
			}
		}
		switch(printops.base) {
			case BASE_BINARY: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_output_binary")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_output_other")), FALSE);
				break;
			}
			case BASE_OCTAL: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_output_octal")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_output_other")), FALSE);
				break;
			}
			case BASE_DECIMAL: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_output_decimal")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_output_other")), FALSE);
				break;
			}
			case BASE_HEXADECIMAL: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_output_hexadecimal")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_output_other")), FALSE);
				break;
			}
			case BASE_SEXAGESIMAL: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_output_sexagesimal")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_output_other")), FALSE);
				break;
			}
			case BASE_TIME: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_output_time")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_output_other")), FALSE);
				break;
			}
			case BASE_ROMAN_NUMERALS: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_output_roman")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_output_other")), FALSE);
				break;
			}
			default: {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_radiobutton_output_other")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_output_other")), TRUE);
				gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(setbase_builder, "set_base_spinbutton_output_other")), printops.base);
			}
		}

		gtk_builder_connect_signals(setbase_builder, NULL);
	
	}

	return GTK_WIDGET(gtk_builder_get_object(setbase_builder, "set_base_dialog"));
}

GtkWidget*
get_nbases_dialog (void)
{
	if(!nbases_builder) {
	
		nbases_builder = getBuilder("nbases.ui");
		g_assert(nbases_builder != NULL);
	
		g_assert (gtk_builder_get_object(nbases_builder, "nbases_dialog") != NULL);
		
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(nbases_builder, "nbases_entry_binary")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(nbases_builder, "nbases_entry_octal")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(nbases_builder, "nbases_entry_decimal")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(nbases_builder, "nbases_entry_hexadecimal")), 1.0);
		
		gtk_builder_connect_signals(nbases_builder, NULL);
		
	}

	return GTK_WIDGET(gtk_builder_get_object(nbases_builder, "nbases_dialog"));
}
GtkWidget*
get_percentage_dialog (void)
{
	if(!percentage_builder) {
	
		percentage_builder = getBuilder("percentage.ui");
		g_assert(percentage_builder != NULL);
	
		g_assert (gtk_builder_get_object(percentage_builder, "percentage_dialog") != NULL);
		
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(percentage_builder, "percentage_entry_1")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(percentage_builder, "percentage_entry_2")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(percentage_builder, "percentage_entry_3")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(percentage_builder, "percentage_entry_4")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(percentage_builder, "percentage_entry_5")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(percentage_builder, "percentage_entry_6")), 1.0);
		gtk_entry_set_alignment(GTK_ENTRY(gtk_builder_get_object(percentage_builder, "percentage_entry_7")), 1.0);

#if GTK_MAJOR_VERSION > 3 || GTK_MINOR_VERSION >= 18
		gtk_text_view_set_left_margin(GTK_TEXT_VIEW(gtk_builder_get_object(percentage_builder, "percentage_description")), 12);
		gtk_text_view_set_right_margin(GTK_TEXT_VIEW(gtk_builder_get_object(percentage_builder, "percentage_description")), 12);
		gtk_text_view_set_top_margin(GTK_TEXT_VIEW(gtk_builder_get_object(percentage_builder, "percentage_description")), 12);
		gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(gtk_builder_get_object(percentage_builder, "percentage_description")), 12);
#endif
		
		gtk_builder_connect_signals(percentage_builder, NULL);
		
	}

	return GTK_WIDGET(gtk_builder_get_object(percentage_builder, "percentage_dialog"));
}

GtkWidget *create_InfoWidget(const gchar *text) {

	GtkWidget *hbox, *image, *infolabel;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_show(hbox);
	gtk_widget_set_halign(hbox, GTK_ALIGN_START);
	//gtk_widget_set_valign(hbox, GTK_ALIGN_CENTER);

	image = gtk_image_new_from_icon_name("dialog-information", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image);
	gtk_box_pack_start (GTK_BOX(hbox), image, FALSE, TRUE, 0);

	infolabel = gtk_label_new(text);
	gtk_widget_show(infolabel);
	gtk_box_pack_start(GTK_BOX(hbox), infolabel, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(infolabel), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(infolabel), TRUE);

	return hbox;
}

GtkWidget* get_argument_rules_dialog (void) {
	
	if(!argumentrules_builder) {
	
		argumentrules_builder = getBuilder("argumentrules.ui");
		g_assert(argumentrules_builder != NULL);
	
		g_assert (gtk_builder_get_object(argumentrules_builder, "argument_rules_dialog") != NULL);
		
		gtk_builder_connect_signals(argumentrules_builder, NULL);
	
	}

	return GTK_WIDGET(gtk_builder_get_object(argumentrules_builder, "argument_rules_dialog"));
}
GtkWidget* get_decimals_dialog (void) {
	if(!decimals_builder) {
	
		decimals_builder = getBuilder("decimals.ui");
		g_assert(decimals_builder != NULL);
	
		g_assert (gtk_builder_get_object(decimals_builder, "decimals_dialog") != NULL);
		
		gtk_builder_connect_signals(decimals_builder, NULL);
	
	}

	return GTK_WIDGET(gtk_builder_get_object(decimals_builder, "decimals_dialog"));
}
GtkWidget* get_plot_dialog (void) {
	if(!plot_builder) {
	
		plot_builder = getBuilder("plot.ui");
		g_assert(plot_builder != NULL);
	
		g_assert (gtk_builder_get_object(plot_builder, "plot_dialog") != NULL);
		
		tPlotFunctions = GTK_WIDGET(gtk_builder_get_object(plot_builder, "plot_treeview_data"));
		tPlotFunctions_store = gtk_list_store_new(10, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_POINTER, G_TYPE_POINTER, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tPlotFunctions), GTK_TREE_MODEL(tPlotFunctions_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tPlotFunctions));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Title"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tPlotFunctions), column);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Expression"), renderer, "text", 1, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tPlotFunctions), column);	
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tPlotFunctions_selection_changed), NULL);
		
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(plot_builder, "plot_button_save")), false);

		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(plot_builder, "plot_combobox_style")), 0);
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(plot_builder, "plot_combobox_smoothing")), 0);
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(plot_builder, "plot_combobox_legend_place")), 0);

		gtk_builder_connect_signals(plot_builder, NULL);
		
	}
		
	return GTK_WIDGET(gtk_builder_get_object(plot_builder, "plot_dialog"));
}
GtkWidget* get_precision_dialog (void) {
	if(!precision_builder) {
	
		precision_builder = getBuilder("precision.ui");
		g_assert(precision_builder != NULL);
	
		g_assert (gtk_builder_get_object(precision_builder, "precision_dialog") != NULL);
		
		gtk_builder_connect_signals(precision_builder, NULL);
	
	}

	return GTK_WIDGET(gtk_builder_get_object(precision_builder, "precision_dialog"));
}
GtkWidget* get_unit_dialog (void) {

	if(!unit_builder) {
	
		unit_builder = getBuilder("unit.ui");
		g_assert(unit_builder != NULL);
		
		g_assert (gtk_builder_get_object(unit_builder, "unit_dialog") != NULL);
		
		tUnitSelectorCategories = GTK_WIDGET(gtk_builder_get_object(unit_builder, "unit_dialog_treeview_category"));
		tUnitSelector		= GTK_WIDGET(gtk_builder_get_object(unit_builder, "unit_dialog_treeview_unit"));
	
		tUnitSelector_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tUnitSelector_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tUnitSelector_store), 0, GTK_SORT_ASCENDING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tUnitSelector), GTK_TREE_MODEL(tUnitSelector_store));
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tUnitSelector));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", 0, NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tUnitSelector), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tUnitSelector_selection_changed), NULL);
		gtk_tree_view_set_enable_search(GTK_TREE_VIEW(tUnitSelector), TRUE);

		tUnitSelectorCategories_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(tUnitSelectorCategories), GTK_TREE_MODEL(tUnitSelectorCategories_store));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tUnitSelectorCategories));
		gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(_("Category"), renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tUnitSelectorCategories), column);
		g_signal_connect((gpointer) selection, "changed", G_CALLBACK(on_tUnitSelectorCategories_selection_changed), NULL);
		gtk_tree_view_column_set_sort_column_id(column, 0);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(tUnitSelectorCategories_store), 0, string_sort_func, GINT_TO_POINTER(0), NULL);
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(tUnitSelectorCategories_store), 0, GTK_SORT_ASCENDING);
		
		gtk_builder_connect_signals(unit_builder, NULL);
		
		update_unit_selector_tree();
	
	}
	gtk_widget_grab_focus(GTK_WIDGET(gtk_builder_get_object(unit_builder, "unit_dialog_entry_unit")));
	return GTK_WIDGET(gtk_builder_get_object(unit_builder, "unit_dialog"));
}
GtkWidget* get_periodic_dialog (void) {
	if(!periodictable_builder) {
	
		periodictable_builder = getBuilder("periodictable.ui");
		g_assert(periodictable_builder != NULL);
	
		g_assert (gtk_builder_get_object(periodictable_builder, "periodic_dialog") != NULL);
		
		gtk_builder_connect_signals(periodictable_builder, NULL);
		
		DataSet *dc = CALCULATOR->getDataSet("atom");
		if(!dc) {
			return GTK_WIDGET(gtk_builder_get_object(periodictable_builder, "periodic_dialog"));
		}
		
		DataObject *e;
		GtkWidget *e_button;
		GtkGrid *e_table = GTK_GRID(gtk_builder_get_object(periodictable_builder, "periodic_table"));
		string tip;
		GtkCssProvider *e_style[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
		GtkCssProvider *l_style = NULL;
		DataProperty *p_xpos = dc->getProperty("x_pos");
		DataProperty *p_ypos = dc->getProperty("y_pos");
		DataProperty *p_weight = dc->getProperty("weight");
		DataProperty *p_number = dc->getProperty("number");
		DataProperty *p_symbol = dc->getProperty("symbol");
		DataProperty *p_class = dc->getProperty("class");
		DataProperty *p_name = dc->getProperty("name");
		int x_pos = 0, y_pos = 0, group = 0;
		string weight;
		l_style = gtk_css_provider_new();
		for(size_t i3 = 0; i3 < 12; i3++) {
			e_style[i3] = gtk_css_provider_new();
		}
		for(size_t i = 1; i < 120; i++) {
			e = dc->getObject(i2s(i));
			if(e) {
				x_pos = s2i(e->getProperty(p_xpos));
				y_pos = s2i(e->getProperty(p_ypos));
			}
			if(e && x_pos > 0 && x_pos <= 18 && y_pos > 0 && y_pos <= 10) {
				e_button = gtk_button_new();
				gtk_button_set_relief(GTK_BUTTON(e_button), GTK_RELIEF_HALF);
				gtk_container_add(GTK_CONTAINER(e_button), gtk_label_new(e->getProperty(p_symbol).c_str()));
				group = s2i(e->getProperty(p_class));
				if(group > 0 && group <= 11) gtk_style_context_add_provider(gtk_widget_get_style_context(e_button), GTK_STYLE_PROVIDER(e_style[group - 1]), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				else gtk_style_context_add_provider(gtk_widget_get_style_context(e_button), GTK_STYLE_PROVIDER(e_style[11]), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				if(x_pos > 2) gtk_grid_attach(e_table, e_button, x_pos + 1, y_pos, 1, 1);
				else gtk_grid_attach(e_table, e_button, x_pos, y_pos, 1, 1);
				tip = e->getProperty(p_number);
				tip += " ";
				tip += e->getProperty(p_name);
				weight = e->getPropertyDisplayString(p_weight);
				if(!weight.empty() && weight != "-") {
					tip += "\n";
					tip += weight;
				}
				gtk_widget_set_tooltip_text(e_button, tip.c_str());
				gtk_widget_show_all(e_button);
				g_signal_connect((gpointer) e_button, "clicked", G_CALLBACK(on_element_button_clicked), (gpointer) e);
			}
		}
		gtk_css_provider_load_from_data(l_style, "* {color: #000000;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[0], "* {color: #000000; background-image: none; background-color: #eeccee;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[1], "* {color: #000000; background-image: none; background-color: #ddccee;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[2], "* {color: #000000; background-image: none; background-color: #ccddff;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[3], "* {color: #000000; background-image: none; background-color: #ddeeff;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[4], "* {color: #000000; background-image: none; background-color: #cceeee;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[5], "* {color: #000000; background-image: none; background-color: #bbffbb;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[6], "* {color: #000000; background-image: none; background-color: #eeffdd;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[7], "* {color: #000000; background-image: none; background-color: #ffffaa;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[8], "* {color: #000000; background-image: none; background-color: #ffddaa;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[9], "* {color: #000000; background-image: none; background-color: #ffccdd;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[10], "* {color: #000000; background-image: none; background-color: #aaeedd;}", -1, NULL);
		gtk_css_provider_load_from_data(e_style[11], "* {color: #000000; background-image: none;}", -1, NULL);
	}

	return GTK_WIDGET(gtk_builder_get_object(periodictable_builder, "periodic_dialog"));
}
