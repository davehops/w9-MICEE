<?php
/*
 * @file micee_csv.module
 *
 */
 
function micee_csv_load_schema($wildcard){
  return drupal_get_form('micee_csv_upload');
}

function micee_csv_upload($form, &$form_state) {
  $form = array();

  $form['choice'] = array(
    '#type' => 'managed_file',
    '#title' => t('MICEE Schema csv data upload'),  // array('@name' => $user->name)),
    '#description' => t('Choose a file to upload. Only CSV files which follow the MICEE template will be acceptable for upload.'),
    '#upload_location' => 'public://tmp/',
    '#upload_validators' => array(
      'file_validate_extensions' => array('csv'),
    ),
  );

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(1),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
	
  return $form;
}


/**
 * Submit handler for module_name_import_form()
 */
function micee_csv_upload_submit($form, &$form_state) {
  /*dsm($form_state['values']);
  $form_state['redirect'] = '';*/
	global $user;
	
  // Check to make sure that the file was uploaded to the server properly
  $uri = db_query("SELECT uri FROM {file_managed} WHERE fid = :fid", array(
    ':fid' => $form_state['input']['choice']['fid'],
  ))->fetchField();
  if(!empty($uri)) {
    if(file_exists(drupal_realpath($uri))) { 
      // Open the csv
      $handle = fopen(drupal_realpath($uri), "r");
      // Go through each row in the csv and run a function on it. In this case we are parsing by '|' (pipe) characters.
      // If you want commas are any other character, replace the pipe with it.
      $edit = $form_state['input']['nid'];
			$alert = "";
			
			$n = node_load($edit);
			
			while (($data = fgetcsv($handle, 0, ',', '"')) !== FALSE) {
				$subject = $data[0];
				$pattern = "/micee/i";
				
				if (preg_match($pattern, $subject)){
					$n->$data[0] = $data[1];
				}else{
					$alert .= " :".$data[0].": "; // concatenate string message to collect fields which are not processed
				}
						
      }
			// Once everything is gathered and ready to be processed... well... process it!
     	node_save($n);
			
			drupal_set_message(t('Congratulations @name, you imported a file successfully. Click here to view @node with your latest revisions. The following fields have been ignored: "@missed" ', array('@name' => $user->name, '@missed' => $alert, '@node' => $edit)), 'status'); 
      fclose($handle);    
    }
  }
  else {
    drupal_set_message(t('There was an error uploading your file. Please contact a System administator.'), 'error');
  }

	
}
