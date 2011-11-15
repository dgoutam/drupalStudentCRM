<?php
/**
 * @file
 * student_crm_activity.relation_type_default.inc
 */

/**
 * Implements hook_relation_default_relation_types().
 */
function student_crm_activity_relation_default_relation_types() {
  $export = array();

  $relation_type = new stdClass;
  $relation_type->disabled = FALSE; /* Edit this to true to make a default relation_type disabled initially */
  $relation_type->api_version = 1;
  $relation_type->relation_type = 'crm_activity';
  $relation_type->label = 'crm_activity';
  $relation_type->reverse_label = 'crm_activity';
  $relation_type->directional = 1;
  $relation_type->transitive = 0;
  $relation_type->r_unique = 0;
  $relation_type->min_arity = 2;
  $relation_type->max_arity = 2;
  $relation_type->source_bundles = array(
    0 => 'crm_activity:*',
  );
  $relation_type->target_bundles = array(
    0 => 'crm_contact:*',
    1 => 'crm_case:*',
  );
  $export['crm_activity'] = $relation_type;

  return $export;
}