<?php
function getelella_interface_get_node_mailbox_settings($node_mailbox){
  $settings = false;
  if(isset($node_mailbox->field_reference_mailbox['und'])
    && isset($node_mailbox->field_reference_mailbox['und'][0]['mailbox'])
  ){
    $nom_mail_ref = $node_mailbox->field_reference_mailbox['und'][0]['mailbox'];
    $feed_mail = feeds_importer($nom_mail_ref);
    $query = db_select('mailhandler_mailbox', 'm')
    ->condition('m.mail', $feed_mail->fetcher->id, '=')
    ->fields('m', array('mail', 'settings'));
    $result = $query->execute();
    foreach ($result as $record) {
      $settings = unserialize($record->settings);
    }
  }
  return $settings;
}

function getelella_interface_new_message($form, &$form_state, $nid, $nid_boite, $mail_nid = null) {
  global $gentelella_var;
  if($nid){
    $gentelella_var['context']['nid_groupe'] = $nid;
    $groupe = node_load($nid);
  }


  $old_message = "";
  $reply_to = "";
  $object = "";
  if($mail_nid && is_numeric($mail_nid)){
    $mail_old = node_load($mail_nid);

    //l'ancien message a un identifiant de boite courrielle
    //cette boite courriel a un groupe en parenté get groupe controlant

  }
  if(isset($mail_old)
      && $mail_old
      && isset($mail_old->field_boite_de_courriel)
      && isset($mail_old->field_boite_de_courriel['und'])
      && isset($mail_old->field_boite_de_courriel['und'][0]['target_id'])
  ){
    $nid_boite = $mail_old->field_boite_de_courriel['und'][0]['target_id'];
  }

  if($nid_boite){
    $node_mailbox = node_load($nid_boite);
  }

  $groupe_controleur = false;
  if($node_mailbox){
    $groupe_controleur = sitemap_get_first_group_controleur($node_mailbox);
    $settings_mail = getelella_interface_get_node_mailbox_settings($node_mailbox);
  }

  if(isset($mail_old) && !$nid && $groupe_controleur){
    $gentelella_var['context']['nid_groupe'] = $groupe_controleur->nid;
    $groupe = $groupe_controleur;
  }

  global $tableau_des_groupes_de_contacts;
  $tableau_des_groupes_de_contacts = array();


  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'groupe_de_contactes')
    ->fieldCondition('og_group_ref', 'target_id', $nid)
    ->propertyOrderBy('title', 'ASC');
  $result = $query->execute();
  if(!empty($result)){
    foreach($result['node'] as $nid_groupe => $retour){
      $tableau_des_groupes_de_contacts[$nid_groupe] = array(
        'profondeur' => 0
      );
    }
  }

  $tableau_des_enfants = array();
  get_sub_group_tree($groupe, $tableau_des_enfants);

  $tableau_des_parents = array();
  get_group_upper_branch($groupe, $tableau_des_parents);
  $r = 0;

  foreach($tableau_des_enfants as $id => $element){
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'groupe_de_contactes')
      ->fieldCondition('og_group_ref', 'target_id', $element['noeud']->nid)
      ->propertyOrderBy('title', 'ASC');
    $result = $query->execute();
    if(!empty($result)){
      foreach($result['node'] as $nid_groupe => $retour){
        $tableau_des_groupes_de_contacts[$nid_groupe] = array(
          'profondeur' => $element['profondeur']
        );
      }
    }
  }

  foreach($tableau_des_parents as $id => $element){
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'groupe_de_contactes')
      ->fieldCondition('og_group_ref', 'target_id', $element['noeud']->nid)
      ->fieldCondition('field_reed_permission_child', 'value', 1)
      ->propertyOrderBy('title', 'ASC');
    $result = $query->execute();
    if(!empty($result)){
      foreach($result['node'] as $nid_groupe => $retour){
        $tableau_des_groupes_de_contacts[$nid_groupe] = array(
          'profondeur' => $element['profondeur']
        );
      }
    }
  }
  /*if(array_key_exists(25079, $tableau_des_groupes_de_contacts)){
    $r = 9;
  }*/


  if($groupe_controleur){
    $gentelella_var['context']['nid_groupe'] = $groupe_controleur->nid;
  }

  if(isset($mail_old)){
    if(isset($mail_old->field_email_reply_to_address['und'])
        && isset($mail_old->field_email_reply_to_address['und']['0']['value']
    )){
      $reply_to .= $mail_old->field_email_reply_to_address['und']['0']['value'];
    }
    if(isset($mail_old->body['und'])
        && isset($mail_old->body['und']['0']['value']
    )){
      $old_message .= "<br/><br/>----------------<br/>";
      $old_message .= $mail_old->body['und']['0']['value'];
    }

    if(isset($mail_old->title)){
      $object .= "Re: " . $mail_old->title;
    }
  }

  $boite = node_load($nid_boite);

  sitemap_get_first_group_controleur($boite);



  $tmpform = array();
  $tmpform_state = array();
  $tmpnode = new stdClass();
  $tmpnode->type = 'email';
  // Create the temporary form/state by reference
  field_attach_form('node', $tmpnode, $tmpform, $tmpform_state);
  $form['#parents'] = $tmpform['#parents'];

  $form['#attached'] = $tmpform['#attached'];

  $form['field_contact_to'] = $tmpform['field_contact_to'];
  $form['field_contact_to']['#weight'] = 3;
  $form_state['field']['field_contact_to'] = $tmpform_state['field']['field_contact_to'];




  //$form_state['field']['field_attachements'] = $tmpform_state['field']['field_attachements'];




  $field = field_info_field("field_attachements");
  $instance = field_info_instance("node", "field_attachements", "email");
  $my_field = field_default_form("node", $tmpnode, $field, $instance, "und", array(), $form, $form_state);
  $form += (array) $my_field;


  $field = field_info_field("field_date_d_envois");
  $instance = field_info_instance("node", "field_date_d_envois", "email");
  $my_field = field_default_form("node", $tmpnode, $field, $instance, "und", array(), $form, $form_state);
  $form += (array) $my_field;

  $field = field_info_field("field_envoyer_plus_tard");
  $instance = field_info_instance("node", "field_envoyer_plus_tard", "email");
  $my_field = field_default_form("node", $tmpnode, $field, $instance, "und", array(), $form, $form_state);
  $form += (array) $my_field;





  $form['field_envoyer_plus_tard']['#weight'] = 3;
  $form['field_date_d_envois']['#weight'] = 4;
  $form['object'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => $object,
    '#size' => 60,
    '#maxlength' => 128,
    '#weight' => 1,
  );
  $form['to'] = array(
    '#type' => 'textfield',
    '#title' => t('to'),
    '#default_value' => $reply_to,
    '#size' => 60,
    '#weight' => 2,
    '#maxlength' => 128,
    '#required' => False,
  );

  /*$form['cc'] = array(
    '#type' => 'textfield',
    '#title' => t('CC'),
    '#default_value' => $reply_to,
    '#size' => 60,
    '#weight' => 4,
    '#maxlength' => 128,
    '#required' => False,
  );

  $form['field_contact_cc'] = $tmpform['field_contact_cc'];
  $form['field_contact_cc']['#weight'] = 5;
  $form_state['field']['field_contact_cc'] = $tmpform_state['field']['field_contact_cc'];

  $form['cci'] = array(
    '#type' => 'textfield',
    '#title' => t('CCI'),
    '#default_value' => $reply_to,
    '#size' => 60,
    '#weight' => 6,
    '#maxlength' => 128,
    '#required' => False,
  );

  $form['field_contact_bcc'] = $tmpform['field_contact_bcc'];
  $form['field_contact_bcc']['#weight'] = 7;
  $form_state['field']['field_contact_cc'] = $tmpform_state['field']['field_contact_bcc'];
  */

  $option_from = array();

  if(isset($node_mailbox->field_vrai_adresse_email['und'])
      && isset($node_mailbox->field_vrai_adresse_email['und'][0]['email'])
  ){
    $option_from = drupal_map_assoc(
        array(
          $node_mailbox->field_vrai_adresse_email['und'][0]['email'],
        )
     );
  }else if($settings_mail){
    $option_from = drupal_map_assoc(
        array(
          $settings_mail['name'],
        )
     );
  }


  $form['from'] = array(
    '#type' => 'select',
    '#title' => 'from',
    '#weight' => 10,
    '#options' => $option_from,
  );
  if($settings_mail){
    //$form['from']['#disabled'] = true;
  }
  $form['message'] = array(
    '#type' => 'text_format',
    '#title' => t('Message'),
    '#default_value' => $old_message,
    '#format' => 'full_html',
    '#weight' => 11,
  );

  $form['field_attachements']['#weight'] = 12;


  /*$form['images'] = array(
    '#type' => 'managed_file',
    '#upload_location' => 'private://images/',
    '#multiple' => TRUE,
    '#upload_validators' => array(
      'file_validate_extensions' => array('png gif jpg jpeg'),
      'file_validate_size' => array(25600000),
      'file_validate_image_resolution' => array('800x600', '400x300'),
    ),
  );*/


  /*$form['my_element'] = array(
    '#type' => 'plupload',
    '#title' => t('Upload files'),
    '#description' => t('This multi-upload widget uses Plupload library.'),
    '#upload_validators' => array(
      'file_validate_extensions' => array('jpg jpeg gif png txt doc xls pdf ppt pps odt  ods odp'),
      'my_custom_file_validator' => array('some validation criteria'),
    ),
    '#plupload_settings' => array(
      'runtimes' => 'html5',
      'chunk_size' => '1mb',
    ),
  );*/



  if($nid_boite){
    $form['nid_mailbox'] = array('#type' => 'hidden', '#value' => $nid_boite);
  }
  if(isset($mail_old) && $mail_old){
    $form['nid_old_message'] = array('#type' => 'hidden', '#value' => $mail_old->nid);
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 13,
  );

  //$form['#after_build'][] = "field_test_after_build_callback";
  $form_state['values_name_field_test_after_build']['field_contact_to'] = array();
  $form_state['values']['field_noeud_parent_sitemap']['und'][0] = $nid_groupe;

  return field_test_after_build_callback($form, $form_state);
}

function getelella_interface_mail_alter(&$message){
/*  if(isset($message['params']['headers']['Reply-To'])){
    $message['headers']['Reply-To'] = $message['params']['headers']['Reply-To'];
  }
  if(isset($message['params']['headers']['Reply-To'])){
    $message['headers']['From'] = $message['params']['headers']['From'];
  }
  if(isset($message['params']['headers']['Reply-To'])){
    $message['headers']['Sender'] = $message['params']['headers']['Sender'];
  }
  if(isset($message['params']['headers']['Reply-To'])){
    $message['headers']['Return-Path'] = $message['params']['headers']['Return-Path'];
  }*/
  if (isset($message['params']['attachments'])) {
    /*foreach($message['params']['attachments'] as $cle => $ligne_attachement){
      $t = 0;
      $vrai_chemin = drupal_realpath($ligne_attachement->uri);
       $file_content = file_get_contents($vrai_chemin);
      $attachments[] = array(
         'filecontent' => $file_content,
         'filename' => 'CV Pierre-Luc.docx',
         'filemime' => 'application/pdf',
       );
    }*/
    //$message['attachments'] = $attachments;
    $message['attachments'] = $message['params']['attachments'];
  }
}

function gentelella_interface_get_email_de_contact($contact, &$tableau_emails_to){
  if(
    empty($contact->field_utilisateur_du_contact)
    && isset($contact->field_email['und'])
    && isset($contact->field_email['und'][0]['email'])
    && !in_array($contact->field_email['und'][0]['email'], $tableau_emails_to)
  ){
    $tableau_emails_to[] = $contact->field_email['und'][0]['email'];
    $t = 0;
  }
  if($contact->nid == 25102
    && isset($contact->field_utilisateur_du_contact['und'])
    && isset($contact->field_utilisateur_du_contact['und'][0]['target_id'])
    && $user_contact = user_load($contact->field_utilisateur_du_contact['und'][0]['target_id'])
  ){
    //on entre dans ce que je veux
    if(isset($user_contact->mail) && !in_array($user_contact->mail, $tableau_emails_to)){
      $tableau_emails_to[] = $user_contact->mail;
    }
    $t = 0;
  }
}
function gentelella_interface_get_contact_de_groupe_de_contact($nid){
  $liste_des_contacts = array();
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
  ->entityCondition('bundle', 'maillon_contenu')
  ->fieldCondition('field_noeud_parent_sitemap', 'target_id', $nid)
  ->propertyOrderBy('title', 'ASC');
  $result = $query->execute();
  if(isset($result['node'])){
    foreach($result['node'] as $nid_maillon => $row){
      $maillon = node_load($nid_maillon);
      if(isset($maillon->field_contenu_refere['und'])){
        foreach($maillon->field_contenu_refere['und'] as $cle => $row){
          if($contact = node_load($row['target_id'])){
            $liste_des_contacts[] = $contact;
          }
        }
      }
    }
  }
  return $liste_des_contacts;
}

function gentelella_interface_get_mail_de_groupe_de_contact($nid_groupe_de_contact){
  $tableau_emails_to = array();
  $liste_contactes = gentelella_interface_get_contact_de_groupe_de_contact($nid_groupe_de_contact);
  foreach($liste_contactes as $nid_contact => $contact){
    gentelella_interface_get_email_de_contact($contact, $tableau_emails_to);
  }
  return $tableau_emails_to;
}

function getelella_interface_new_message_submit($form, &$form_state){
  global $user;
  $from = $form_state['values']['from'];
  $sendto	= $form_state['values']['to'];


  if(!isset($form_state['values']['nid_mailbox'])
      || !($mailbox = node_load($form_state['values']['nid_mailbox']))
  ){
    $mailbox = false;
  }

  $tableau_email_to = array();
  $field_contact_to = $form_state['values']['field_contact_to'];
  if(isset($field_contact_to['und'])){
    foreach($field_contact_to['und'] as $cle => $reference){
      $tableau_email_to = gentelella_interface_get_mail_de_groupe_de_contact($reference['target_id']);
    }
  }
  foreach(array_map('trim', explode(',', $sendto)) as $cle => $chaine){
    if(empty($tableau_email_to) || !in_array($chaine, $tableau_email_to)){
      $tableau_email_to[] = $chaine;
    }
  }
  $sendto = implode($tableau_email_to, ", ");

  /*$field_contact_cc = $form_state['values']['field_contact_cc'];
  if(isset($field_contact_cc['und'])){
    foreach($field_contact_cc['und'] as $cle => $reference){
      $tableau_email_cc = gentelella_interface_get_mail_de_groupe_de_contact($reference['target_id']);
    }
  }

  $field_contact_bcc = $form_state['values']['field_contact_bcc'];
  if(isset($field_contact_bcc['und'])){
    foreach($field_contact_bcc['und'] as $cle => $reference){
      $tableau_email_bcc = gentelella_interface_get_mail_de_groupe_de_contact($reference['target_id']);
    }
  }*/


  $node = entity_create('node', array('type' => 'email'));
  // Specify the author.
  $node->uid = $user->uid;
  // Create a Entity Wrapper of that new Entity.
  $emw_node = entity_metadata_wrapper('node', $node);
  // Set a title and some text field value.
  $emw_node->title = $form_state['values']['object'];
  $emw_node->field_email_to_address = $sendto;
  $emw_node->body->set(array(
    'value' => $form_state['values']['message']['value'],
    'summary' => $form_state['values']['message']['value'],
    'format' => 'full_html',
  ));

  if($mailbox){
    $emw_node->field_boite_de_courriel->set($mailbox->nid);
    $emw_node->field_prenom = $mailbox->title;
  }
  if(isset($form_state['values']['nid_old_message']) && $form_state['values']['nid_old_message']){
    $emw_node->field_mail_en_reponse_de->set($form_state['values']['nid_old_message']);
  }
  $emw_node->field_email_from_here = true;

  $emw_node->field_email_reply_to_address = $from;
  $emw_node->field_email_from_address = $from;


  if(isset($form_state['values']['field_date_d_envois'])
      && isset($form_state['values']['field_date_d_envois']['und'][0])
  ){
    $emw_node->field_date_d_envois->set($form_state['values']['field_date_d_envois']['und'][0]['value']);
  }
  $emw_node->field_envoyer_plus_tard->set($form_state['values']['field_envoyer_plus_tard']['und'][0]['value']);



  if(isset($form_state['values']['field_attachements']['und'])){
    foreach($form_state['values']['field_attachements']['und'] as $cle => $ligne_fichier){
      if ($file = file_load($ligne_fichier['fid'])) {
        $file->status = FILE_STATUS_PERMANENT;
        file_save($file);

        // Attach the file to the node.
        $emw_node->field_attachements[] = array(
          'fid' => $file->fid,
          'display' => TRUE,
          'description' => $file->filename,
        );
      }
    }
  }
  $emw_node->save();
  if(gentelella_interface_si_mail_est_a_envoyer($emw_node->getIdentifier())){
    gentelella_interface_envoyer_email($emw_node->getIdentifier());
  }
}




function sendgrid_integration_mail_alter(&$message){
  if (isset($message['params']['attachments'])) {
    $message['attachments'] = $message['params']['attachments'];
  }
}
