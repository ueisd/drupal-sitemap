<?php

namespace Drupal\hello\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of the 'color_picker_widget' widget.
 *
 * @FieldWidget(
 *   id = "color_picker_widget",
 *   label = @Translation("Color picker widget"),
 *   field_types = {
 *     "string"
 *   }
 * )
 */
class ColorPickerWidget extends WidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element['value'] = $element +
      [
        '#type' => 'color',
        '#default_value' => isset($items[$delta]->value) ? $items[$delta]->value : NULL,
      ];

    return $element;
  }

}
