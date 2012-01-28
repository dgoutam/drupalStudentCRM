<?php

/**
 * @file
 * A basic template for activity entities
 *
 * Available variables:
 * - $content: An array of comment items. Use render($content) to print them all, or
 *   print a subset such as render($content['field_example']). Use
 *   hide($content['field_example']) to temporarily suppress the printing of a
 *   given element.
 * - $title: The name of the activity
 * - $url: The standard URL for viewing a activity entity
 * - $page: TRUE if this is the main view page $url points too.
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. By default the following classes are available, where
 *   the parts enclosed by {} are replaced by the appropriate values:
 *   - entity-profile
 *   - activity-{TYPE}
 *
 * Other variables:
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 *
 * @see template_preprocess()
 * @see template_preprocess_entity()
 * @see template_process()
 */
?>
<div class="<?php print $classes; ?> activity clearfix"<?php print $attributes; ?>>
  <div class="meta">
    <?php print $picture; ?>
    <div class="activity-type">
      <?php print $contact_type; ?>
    </div>
    <?php if($status_change): ?>
    	<?php print render($status_change); ?>
    <?php endif; ?>
  </div>
  
  <div class="content"<?php print $content_attributes; ?>>
  <div class="submitted"><?php print $submitted; ?> - <?php print $permalink; ?></div>  
    <div class="note">
       <?php
          print render($content);
        ?>
    </div>
  </div>
</div>
