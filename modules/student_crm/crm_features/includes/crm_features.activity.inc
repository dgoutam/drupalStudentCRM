<?php

/**
 * @file
 * CRM Activity features hooks.
 */
 
/**
 * Implements hook_features_export_options().
 */
function crm_activity_features_export_options() {
  $names = array();
  foreach (crm_core_activity_types() as $type) {
    $names[$type->type] = $type->label;
  }
  return $names;
}

/**
 * Implements hook_features_export.
 */
function crm_activity_features_export($data, &$export, $module_name = '') {
  $pipe = array();
  $map = features_get_default_map('crm_core_activity');

  foreach ($data as $type) {
    $info = FALSE;
    foreach (crm_core_activity_types() as $type_info) {
      if ($type == $type_info->type) {
        $info = $type_info;
      }
    }
    if ($info) {
      $export['features']['crm_core_activity'][$type] = $type;
      $export['dependencies']['crm'] = 'crm';
      $export['dependencies']['features'] = 'features';
      
      $fields = field_info_instances('crm_core_activity', $type);
      foreach ($fields as $name => $field) {
        $pipe['field'][] = "crm_activity-{$field['bundle']}-{$field['field_name']}";
      }
    }
  }

  return $pipe;
}

/**
 * Implements hook_features_export_render().
 */
function crm_activity_features_export_render($module, $data, $export = NULL) {
  $elements = array(
    'type' => TRUE,
    'label' => FALSE,
    'description' => TRUE,
  );
  $output = array();
  $output[] = '  $items = array(';
  foreach ($data as $type) {
    if ($info = crm_core_activity_types($type)) {
      $output[] = "    '{$type}' => array(";
      foreach ($elements as $key => $t) {
        if ($t) {
          $text = str_replace("'", "\'", $info->$key);
          $text = !empty($text) ? "t('{$text}')" : "''";
          $output[] = "      '{$key}' => {$text},";
        }
        else {
          $output[] = "      '{$key}' => '{$info->$key}',";
        }
      }
      $output[] = "    ),";
    }
  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  return array('crm_features_activity_defaults' => $output);
}

/**
 * Implements hook_features_revert().
 *
 * @param $module
 * name of module to revert content for
 */
function crm_activity_features_revert($module = NULL) {
  if ($default_types = features_get_default('crm_core_activity', $module)) {
    foreach ($default_types as $type_name => $type_info) {
      db_delete('crm_activity_type')
        ->condition('type', $type_name)
        ->execute();
    }
    crm_activity_features_enable($module);
    cache_clear_all();
    menu_rebuild();
  }
}

/**
 * Implements hook_features_disable().
 *
 * @param $module
 *   Name of module that has been disabled.
 */
function crm_activity_features_disable($module) {

}

/**
 * Implements hook_features_enable().
 *
 * @param $module
 *   Name of module that has been enabled.
 */
function crm_activity_features_enable($module) {
  if ($default_types = features_get_default('crm_core_activity', $module)) {
    foreach ($default_types as $type_name => $type_info) {
      $type_info = (object)$type_info;
      // Ensure the type exists.
      if ($existing_type = crm_activity_type_load($type_name)) {
        $existing_type = (object)$existing_type;
        $existing_type->custom = 0;
        crm_activity_type_save($type_info);
      }
      else {
        $type_info->is_new = TRUE;
        crm_activity_type_save($type_info);
      }
    }
  }
}
