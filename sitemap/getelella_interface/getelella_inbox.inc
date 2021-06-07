<?php
function getelella_interface_inbox($nid, $nid_boite_courriel=false){
  global $gentelella_var;
  $gentelella_var['context']['nid_groupe'] = $nid;
  //drupal_add_library("jscroll_library", "jscroll");
  //drupal_add_library("custom_scrollbar_library", "custom_scrollbar");

  /*$view = views_get_view('liste_des_emails');
  $view->set_display('block_1');
  $view->get_title();
  $response = $view->preview('ceci_est_un_test');*/
  drupal_add_css(drupal_get_path('module', 'getelella_interface') . "/css/loader.css");
  drupal_add_js(drupal_get_path('module', 'getelella_interface') . "/js/boite_courielle.js");
  $adresse = drupal_get_path_alias();
  $adresse_creer_courriel = url('dashboard/' . $nid . '/inbox/' . $nid_boite_courriel . '/newmessage', array('absolute' => TRUE, 'query' => array('destination'=>$adresse)));
  gd_infinite_scroll_add_unique_settings('tes_list');
  $output = "";

  if($nid_boite_courriel && ($boite_de_courriel = node_load($nid_boite_courriel))){
    $titre_boite_de_courriel = $boite_de_courriel->title;
  }else{
    $titre_boite_de_courriel = "vide";
  }
  if($nid && ($groupe = node_load($nid))){
    $titre_du_groupe = $groupe->title;
  }else{
    $titre_du_groupe = "vide";
  }

  $output .= '<div class="">
              <input type="hidden" name="nid_boite_courriel" value="' . $nid_boite_courriel . '"/>
              <input type="hidden" name="adresse_globale" value="' . $adresse . '"/>
            <div class="page-title">
              <div class="title_left">
                <h3>Goupe <small>' . $titre_du_groupe . '</small></h3>
              </div>

              <div class="title_right">
                <div class="col-md-5 col-sm-5 col-xs-12 form-group pull-right top_search">
                  <div class="input-group">
                    <input type="text" class="form-control" placeholder="Rechercher...">
                    <span class="input-group-btn">
                      <button class="btn btn-default" type="button">Aller!</button>
                    </span>
                  </div>
                </div>
              </div>
            </div>

            <div class="clearfix"></div>

            <div class="row">
              <div class="col-md-12">
                <div class="x_panel">
                  <div class="x_title">
                    <h2>Boîte de courriel<small>' . $titre_boite_de_courriel . '</small></h2>
                    <ul class="nav navbar-right panel_toolbox">
                      <li><a class="collapse-link"><i class="fa fa-chevron-up"></i></a>
                      </li>
                      <li class="dropdown">
                        <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-expanded="false"><i class="fa fa-wrench"></i></a>
                        <ul class="dropdown-menu" role="menu">
                          <li><a href="#">Settings 1</a>
                          </li>
                          <li><a href="#">Settings 2</a>
                          </li>
                        </ul>
                      </li>
                      <li><a class="close-link"><i class="fa fa-close"></i></a>
                      </li>
                    </ul>
                    <div class="clearfix"></div>
                  </div>
                  <div class="x_content">
                    <div class="row">
                      <div class="col-sm-3">
                        <a href="' . $adresse_creer_courriel . '" id="compose" class="btn btn-sm btn-success btn-block" type="button">CRÉÉR UN NOUVEAU COURIEL</a>
                      </div>
                      <div class="col-sm-9">
                            <div class="btn-group" data-toggle="buttons">
                              <label class="btn btn-default active">
                                <input id="option1" name="provenance" type="radio" value="recu">
                                reçu
                              </label>
                              <label class="btn btn-default">
                                <input id="option2" name="provenance" type="radio" value="envoye">
                                envoyé
                              </label>
                              <label class="btn btn-default">
                                <input id="option3" name="provenance" type="radio" value="pending">
                                à envoyer plus tard
                              </label>
                            </div>
                      </div>
                    </div>
                    <div class="row">
                      <div class="col-sm-3 mail_list_column" id="mail_list_column">
                        <div class="content-mail-list">
                        ';
                        if($nid_boite_courriel){
                          $output .= getelella_interface_messageindex($nid_boite_courriel, 0, false);
                        }else{
                          $output .= views_embed_view('liste_des_emails', 'block_1', "");
                        }
                        $output .= '</div>
                      </div>
                      <div id="newsfeed"></div>
                      <!-- /MAIL LIST -->

                      <!-- CONTENT MAIL -->
                      <div class="col-sm-9 mail_view">
                        <div class="inbox-body">
                        </div>

                      </div>
                      <!-- /CONTENT MAIL -->
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>';
  //gd_infinite_scroll_add_unique_settings("tes_list");
  return $output;
}
