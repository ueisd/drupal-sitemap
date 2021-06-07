<?php
function getelella_interface_entity_info_alter(&$entity_info) {
  $entity_info['node']['view modes']['popup_content'] = array(
    'label' => t('Evenement contenu de popup'),
    'custom settings' => TRUE,
  );
}