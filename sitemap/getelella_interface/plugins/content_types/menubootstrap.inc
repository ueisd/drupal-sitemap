<?php
    $plugin = array(
      'single' => true,
      'title' => t('Un menu bootstrap'),
      'description' => t('Permet d\'afficher un menu spécial et conntextuel à l\'intérieur d\'un paneau'),
      'category'        => array('Plugin pour page'),
      'edit form' => 'getelella_interface_menubootstrap_content_type_edit_form',
      'render callback' => 'getelella_interface_menubootstrap_content_type_render',
      'all contexts' => TRUE,
      'required context' => new ctools_context_required(t('Node'), 'node'),
      'title singular' => "menu bootstrap",
      'title plural' => "Menu bootstrap",
      'title singular proper' => "menus bootstraps",
      'title plural proper' => "Menus bootstraps",
      'schema' => 'panelizer_defaults',
    );


     function menubootstrap_schema() {
       $schema['menubootstrap'] = array(
           'description' => t('Table storing preset definitions.'),
           'fields' => array( 'title' => array(
               'type' => 'varchar',
               'length' => '255',
               'description' => 'Unique ID for presets. Used to identify them programmatically.',
              ),
           ),

       );
     }

    /** * Implementation of hook_install(). */
     function menubootstrap_install() {
       drupal_install_schema('ffmeg_converter');
     }


    /**
     * Title callback for admin page.
     */
    function getelella_interface_menubootstrap_admin_title($subtype, $conf, $context = NULL) {
      return t('This is my custom pane menu bootstrap.');
    }

    /**
     * Callback to provide administrative info (the preview in panels when building a panel).
     */
    function getelella_interface_menubootstrap_admin_info($subtype, $conf, $context = NULL) {
      $block = new stdClass();
      $block->title = t('Custom pane');
      $config = array();
      if ($conf['override_title'] == TRUE) {
        $title_value = '<b>' . $conf['override_title_text'] . '</b>';
      }
      else {
        $title_value = t('Not Set');
      }
      $config[] = t('Title') . ': ' . $title_value;
      $block->content = theme_item_list(array( 'items' => $config, 'title' => NULL, 'type' => 'ul', 'attributes' => array() ));
      return $block;
    }

    /**
     * Edit callback for the content type.
     */
    function getelella_interface_menubootstrap_content_type_edit_form($form, &$form_state) {
      $conf = $form_state['conf'];

      if ($form_state['op'] == 'add') {
        $form['nid'] = array(
          '#prefix' => '<div class="no-float">',
          '#title' => t('Enter the title or NID of a node'),
          '#description' => t('To use a NID from the URL, you may use %0, %1, ..., %N to get URL arguments. Or use @0, @1, @2, ..., @N to use arguments passed into the panel.'),
          '#type' => 'textfield',
          '#maxlength' => 512,
          '#autocomplete_path' => 'ctools/autocomplete/node',
          '#weight' => -10,
          '#suffix' => '</div>',
        );
      }
      else {
        $form['nid'] = array(
          '#type' => 'value',
          '#value' => $conf['nid'],
        );
      }

      return $form;
    }

    /**
     * Submit callback for settings form.
     */
    function getelella_interface_menubootstrap_content_type_edit_form_submit($form, &$form_state) {
      foreach (element_children($form) as $key) {
        if (isset($form_state['values'][$key])) {
          $form_state['conf'][$key] = $form_state['values'][$key];
        }
      }
    }


function trier_item_menu_selon_poid_et_titre($sous_pages){
  foreach($sous_pages as $cle => $noeud){
    if($terme_du_noeud = sitemap_get_terme_associe($noeud)){
      $noeud->weight = $terme_du_noeud->weight;
    }
  }
  usort($sous_pages, function($a, $b){
    $t = 0;
    if($a->weight < $b->weight){
      return -1;
    }else if($a->weight > $b->weight){
      return 1;
    }else{
      return strcmp($a->title, $b->title);
    }
  });
}

function get_sous_pages_tries_pour_menu($racine_menu){
  $sous_pages = array_filter(sitemap_get_noeuds_enfants($racine_menu), function($obj){
    if (isset($obj->type) && variable_get('est_page_' . $obj->type, FALSE)) {
      return true;
    }
    return false;
  });

  $sous_pages_archive = array_filter($sous_pages, function($obj){
    if (isset($obj->type)
        && isset($obj->field_statut_archivage['und'][0]['value'])
        && $obj->field_statut_archivage['und'][0]['value'] == 3
    ) {
      return true;
    }
    return false;
  });
  $sous_pages_non_archive = array_filter($sous_pages, function($obj){
    if (isset($obj->type)
        && isset($obj->field_statut_archivage['und'][0]['value'])
        && $obj->field_statut_archivage['und'][0]['value'] != 3
    ) {
      return true;
    }
    return false;
  });
  trier_item_menu_selon_poid_et_titre($sous_pages_non_archive);
  trier_item_menu_selon_poid_et_titre($sous_pages_archive);
  $retour = array();
  foreach($sous_pages_non_archive as $cle => $noeud){
    $retour[] = $noeud;
  }
  foreach($sous_pages_archive as $cle => $noeud){
    $retour[] = $noeud;
  }
  $t = 0;
  return $retour;

}

function affiche_item_menu($noeud){
  $output = '';
  $sous_pages = get_sous_pages_tries_pour_menu($noeud);
  if(isset($noeud->field_racine_d_un_menu_)
    && isset($noeud->field_racine_d_un_menu_['und'][0]['value'])
    && $noeud->field_racine_d_un_menu_['und'][0]['value'] == 1
  ){
    $est_racine = true;
  }else{
    $est_racine = false;
  }

  if(isset($noeud->field_statut_archivage['und'][0]['value'])
      && $noeud->field_statut_archivage['und'][0]['value'] == 3
  ){
    $icone_archives = '<span class="glyphicon glyphicon-folder-close text-warning"></span> ';
  }else{
    $icone_archives = '';
  }

  if($lien_alias = drupal_lookup_path('alias','node/'.$noeud->nid)){
    $lien_du_noeud = $lien_alias;
  }else{
    $lien_du_noeud = 'node/'.$noeud->nid;
  }

  $sous_noeud_url = url($lien_du_noeud, array('absolute' => TRUE));
  $sous_noeud_titre_alt = '(' . $noeud->type . ') - ' . $noeud->title;

  if(empty($sous_pages) || $est_racine){
    $output = '<li><a href="' . $sous_noeud_url . '" title="' . $sous_noeud_titre_alt . '">' . $icone_archives . $noeud->title . '</a></li>';
  }else{
    foreach($sous_pages as $cle => $sous_noeud){
      $output .= affiche_item_menu($sous_noeud);
    }
    $output = '
        <li class="dropdown clearfix">
          <a href="' . $sous_noeud_url . '" title="' . $sous_noeud_titre_alt . '">' . $icone_archives . $noeud->title . '<span class="caret"></span></a>
          <span class="toggle-custom glyphicon glyphicon-plus" data-toggle="collapse" data-target="#submenu' . $noeud->nid . '"></span>
          <ul class="dropdown-menu" id="submenu' . $noeud->nid . '">'
            . $output .
          '</ul>
        </li>
        <div class="clearfix visible-xs"></div>';
  }
  return $output;
}
    /**
     * Run-time rendering of the body of the block (content type).
     */
function getelella_interface_menubootstrap_content_type_render($subtype, $conf, $panel_args, $context) {
  $block = new stdClass();
  drupal_add_js(drupal_get_path('module', 'getelella_interface') . "/test2.js");

  if(isset($panel_args) && isset($panel_args[0]) && ($node_page = node_load($panel_args[0]))){
    if(isset($node_page->field_racine_menu_a_afficher)
        && isset($node_page->field_racine_menu_a_afficher['und'][0])
        && isset($node_page->field_racine_menu_a_afficher['und'][0]['target_id'])
        && ($racine_menu = node_load($node_page->field_racine_menu_a_afficher['und'][0]['target_id']))
    ){
      $sousPages = get_sous_pages_tries_pour_menu($racine_menu);
    }
  }

  $racine_url = url(drupal_lookup_path('alias',"node/".$racine_menu->nid), array('absolute' => TRUE));
  $output = '';
  $output = '<li><a href="' . $racine_url . '"><span class="glyphicon glyphicon-home"></span> ' . $racine_menu->title . '</a></li>';
  foreach($sousPages as $cle => $sous_noeud){
    $output .= affiche_item_menu($sous_noeud);
  }


  $output = '
   <nav class="navbar navbar-default menu-bootsrap-principal navbarPage">
   <div class="container-fluid">
      <div class="navbar-header">
         <button type="button" class="navbar-toggle" data-toggle="collapse" data-target="#navbarPage">
         <span class="icon-bar"></span>
         <span class="icon-bar"></span>
         <span class="icon-bar"></span>
         </button>
      </div>
      <div class="collapse navbar-collapse" id="navbarPage">
         <h2 class="sr-only">Menu principal</h2>
         <ul class="nav navbar-nav links clearfix">
         ' . $output . '
         </ul>
      </div>
   </div>
</nav>';








  if (isset($conf['nid'])) {
    $block = new stdClass();
    if ( $conf['override_title'] == TRUE ) {
      $block->title = $conf['override_title_text'];
    }
    else {
      $block->title = NULL;
    }
    if(isset($context) && isset($context['panelizer']) && isset($context['panelizer']->argument)){
      $node = node_load($context['panelizer']->argument);
    }
    $block->content = $output; //"<h3>ceci est un test</h3>";
    return $block;
  }
  return NULL;
}

