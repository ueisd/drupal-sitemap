<?php

function get_groupe_parent_du_groupe($groupe){
  $w_node = entity_metadata_wrapper('node', $groupe);
  return (isset($w_node->field_heritage_de_groupe)) ?
    $w_node->field_heritage_de_groupe->value() : null;
}

function get_groupe_enfants_du_groupe($groupe){
  $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'groupe')
      ->fieldCondition('field_heritage_de_groupe', 'target_id', $groupe->nid)
      ->propertyOrderBy('title', 'ASC');
  $result = $query->execute();
  return (isset($result['node'])) ? $result['node'] : null;
}

function get_sub_group_tree($groupe, &$result, $profondeur = 0){
  $profondeur++;
  if($liste_groupes_enfants = get_groupe_enfants_du_groupe($groupe)){
    foreach($liste_groupes_enfants as $nid => $entite){
      if($sous_noeud = node_load($nid)){
        $result[] = array('noeud' => $sous_noeud, 'profondeur' => $profondeur);
        get_sub_group_tree($sous_noeud, $result, $profondeur);
      }
    }
  }
}

function get_group_upper_branch($groupe, &$result, $profondeur = 0){
  $profondeur--;
  if($groupe_parent = get_groupe_parent_du_groupe($groupe)){
    $result[] = array('noeud' => $groupe_parent, 'profondeur' => $profondeur);
    get_group_upper_branch($groupe_parent, $result, $profondeur);
  }
}
