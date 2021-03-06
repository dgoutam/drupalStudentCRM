<?php
/**
 * @file
 * student_crm_gpa.relation_type_default.inc
 */

/**
 * Implements hook_relation_default_relation_types().
 */
function student_crm_gpa_relation_default_relation_types() {
  $export = array();

  $relation_type = new stdClass;
  $relation_type->disabled = FALSE; /* Edit this to true to make a default relation_type disabled initially */
  $relation_type->api_version = 1;
  $relation_type->relation_type = 'term_gpa';
  $relation_type->label = 'Term GPA';
  $relation_type->reverse_label = 'term_gpa';
  $relation_type->directional = 1;
  $relation_type->transitive = 0;
  $relation_type->r_unique = 0;
  $relation_type->min_arity = 2;
  $relation_type->max_arity = 2;
  $relation_type->source_bundles = array(
    0 => 'crm_core_contact:student',
  );
  $relation_type->target_bundles = array(
    0 => 'taxonomy_term:term',
  );
  $export['term_gpa'] = $relation_type;

  return $export;
}
