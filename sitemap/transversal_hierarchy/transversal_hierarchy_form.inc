<?php
function form_example_tutorial_1($form, &$form_state, $nid){

  $form['html'] = array(
   '#type' => 'markup',
   '#markup' => '<div id="chart-container"></div>',
  );

  $form['nid'] = array(
    '#type' => 'hidden',
    '#disabled' => true,
    '#attributes' => array('id' => array('nid_de_la_page')),
    '#default_value' =>$nid,
  );

  $form['liste_groupes'] = array(
    '#type' => 'fieldset',
    '#title' => t('test'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#attributes' => array('id' => array('liste_des_groupes_fieldset')),
  );

  $default = array();
  $node = node_load($nid);
  $wrapper = entity_metadata_wrapper('node', $node);
  if(isset($wrapper->groupe_parents_tranversaux)){
    foreach($wrapper->groupe_parents_tranversaux as $delta => $item)  {
      $default[] = $item->nid->value();
    }
  }


  $tab = array();

  foreach(($tous_les_groupes = transversal_hierarchy_get_all_group()) as $nid_tous => $node){
    if($nid_tous != $nid){
      $tab[$nid_tous] = $node->title;
    }
  }
  $form['liste_groupes']['les_transversalise'] = array(
    '#type' => 'checkboxes',
    '#options' => $tab,
    '#attributes' => array('class' => array('liste_de_groupe_checkbox')),
    '#default_value' => $default,
    '#title' => t('Noeud à transversaliser'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  $form['#submit'][] = 'form_example_tutorial_1_submit';


  $form['#attached']['library'][] = array('transversal_hierarchy', 'jorgchart_mit');

  $form['#attached']['js'] = array(
    drupal_get_path('module', 'transversal_hierarchy') . '/dist/js/scripts.js',
  );
  return $form;
}

function form_example_tutorial_1_submit($form, &$form_state){
$ajouter = array();
  $retirer = array();
  foreach($form_state['values']['les_transversalise'] as $cle => $val){
    if($val == 0){
        $retirer[] = $cle;
    }else{
        $ajouter[] = $cle;
    }
  }
  $retirer_net = array();
  $node = node_load($form_state['values']['nid']);
  $wrapper = entity_metadata_wrapper('node', $node);
  foreach($wrapper->groupe_parents_tranversaux as $delta => $item)  {
    $val = $item->nid->value();
    if(in_array($val, $retirer)){
      $retirer_net[] = $val;
      unset($wrapper->groupe_parents_tranversaux[$delta]);
    }
    if(($key = array_search($val, $ajouter)) !== false) {
      unset($ajouter[$key]);
    }
  }
  $retirer = $retirer_net;
  $role_heriteur = og_roles_heriteur('node', $node->type, $node->nid, 1);
  $les_perms = og_role_permissions($role_heriteur);
  foreach($ajouter as $val){
    $wrapper->groupe_parents_tranversaux[] = $val;
    if($noeud_enfant = node_load($val)){
      foreach($role_heriteur as $rid => $role_title){
        $role = new stdClass();
        $role->rid = $rid;
        $role->name = $role_title;
        $role_nouv = copier_role_dans_groupe($node->nid, $noeud_enfant->nid, $role, 1);
      }
    }
  }
  foreach($retirer as $val){
    foreach($role_heriteur as $rid => $role_title){
      $query = db_select('og_role', 'ogr');
      $query->fields('ogr', array('rid', 'name'));
      $query->condition('ogr.gid', $val, '=');
      $query->condition('ogr.parent_rid', $rid, '=');
      $query->condition('ogr.de_transv', 1, '=');
      $result = $query->execute()->fetchAllkeyed();

      $r = 0;
      foreach($result as $cle => $nom_role){
        og_role_delete($cle);
      }
    }
  }
  $wrapper->save();

  /*foreach(array_keys(get_role_sub_trans($node)) as $rid_delete){
    og_role_delete($rid_delete);
  }*/


  $r = 0;
}

function get_role_sub_trans($groupe_parent, $groupe = null){
  $rids = null;
  $nids = array_keys(og_roles_heriteur('node',  $groupe_parent->type, $groupe_parent->nid, 1));
  $query = db_select('og_role', 'ogr');
  $query->fields('ogr', array('rid', 'name'));
  if($groupe){
    $query->condition('ogr.group_bundle', $groupe->type, '=');
    $query->condition('ogr.gid', $groupe->nid, '=');
  }if(!empty($nids)){
    $query->condition('ogr.par_trans', $nids, 'IN');
    $query->orderBy('rid', 'ASC');
    $rids = $query->execute()->fetchAllkeyed();
  }
  return $rids;
}