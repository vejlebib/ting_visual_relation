<?php
/**
 * @file
 * Default implementation of the ting-visual-relation-activation-arae template.
 *
 * Available variables:
 *   - $activators: The relation browser activators to be renderes in this
 *     activation area.
 *   - $attributes: String of attributes that can be used to style contextually
 *     with CSS. It can be manipulated throught $classes_array variable in
 *     preprocess functions.
 *   - $classes: String of classes that can be used to style contextually with
 *     CSS. It can be manipulated throught $classes_array variable in preprocess
 *     functions.
 */
?>

<?php if (!empty($activators)): ?>
  <div class="ting-visual-relation-wrapper <?php print $classes;?>" <?php print $attributes; ?>>
    <div class="ting-visual-relation-inner-wrapper">
      <?php foreach ($activators as $activator): ?>
        <?php print render($activator); ?>
      <?php endforeach; ?>
    </div>
  </div>
<?php endif; ?>
