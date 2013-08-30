<?php
/*
 * @file micee_csv.module
 *
 */
 
function micee_csv_load_schema($wildcard){
  return drupal_get_form('_micee_csv_upload');
}

function _micee_csv_upload($form, &$form_state) {
//  if(!isset($form_state['storage']['confirm'])) {

	
	
		$form['choice'] = array(
			'#type' => 'managed_file',
			'#title' => t('MICEE Schema csv data upload'),  
			'#description' => t('Choose a file to upload. Only CSV files which follow the MICEE template will be acceptable for upload.!brFor more information on working with MICEE CSV files, !url', array('!br' => '<br>', '!url' => l(t('click here.'), "micee-csv-tutorial"))),
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
	//} else {	
//    drupal_set_message('hello2');
//		$question = $n->title;
//		$path = "/node/".$n->nid."/import-csv";
//		$description = t('!code1Are you sure you want to import this CSV file? Import overwrites all entries previously recorded to this publication. This can not be undone.!code2', array('!code1' => '<span style="font-weight:bold; font-size:1.1em; font-style:italic;">', '!code2' => '</span>'));
//		$yes = "Confirmm";
//		$no = "Cancell";
//		$name = "confirm";
//		return confirm_form($form, $question, $path, $description, $yes, $no, $name); //$form, $question, $path, $description = NULL, $yes = NULL, $no = NULL, $name = 'confirm'
//	}
}


/**
 * Submit handler for module_name_import_form()
 */
function _micee_csv_upload_submit($form, &$form_state) {
	global $user;
	//if(!isset($form_state['storage']['confirm'])) {
//    //drupal_set_message('helloooooo'); 
//    $form_state['storage']['confirm'] = TRUE;
//    $form_state['rebuild'] = TRUE;
//  } else {
//	  foreach($form_state as $term){
//  		if (is_array($term)){
//				foreach($term as $sub_term){
//					$output .= $sub_term . ' : ';
//				};
//			} else {
//				$output .= $term . ' # ';
//			}
//		};
//		drupal_set_message("hellooo:".$output.":helloooo");
		//print_r('waht THE f');
		//drupal_set_message($form_state['input']['fid']);
    $uri = db_query("SELECT uri FROM {file_managed} WHERE fid = :fid", array(':fid' => $form_state['input']['choice']['fid'],))->fetchField();
		if(!empty($uri)) {
			if(file_exists(drupal_realpath($uri))) { 
				// Open the csv
				$handle = fopen(drupal_realpath($uri), "r");
				// Go through each row in the csv and run a function on it. In this case we are parsing by ',' (comma) characters.
				$edit = $form_state['input']['nid'];
				$alert = "The following fields have been ignored: ";
				$a = 0;
				
				$n = node_load($edit);
				
				while (($data = fgetcsv($handle, 0, ',', '"')) !== FALSE) {
					$matches = $data[0];
					$pattern = "/^micee[0-9a-z_]+$/";
					if (preg_match($pattern, $matches)){
						if ($data[0] == 'micee1_1_type'){
							$data[1] = unserialize($data[1]);
						}
						if (array_key_exists($data[0], $n)) {  // check to see if the data your pushing has a home...
								$n->$data[0] = $data[1];
						}else{
							$alert .= " :".$data[0].": "; // concatenate string message to collect fields which are not processed
							$a++;
						}
					}else{
						$alert .= " :".$data[0].": "; // concatenate string message to collect fields which are not processed
						$a++;
					}
				}
				if ($a == 0){
					$alert = "";
				}
				// Once everything is gathered and ready to be processed... well... process it!
				//$n->revision = TRUE;
				$n->revision = 1; // Create new revision
				$n->log = t('MICEE Schema Updated programmatically by @name', array('@name' => $user->name)); // Log message
				node_save($n);
				drupal_set_message(t('Congratulations @name, you imported the new schema file successfully. Click view below to see your latest revisions. @missed', array('@name' => $user->name, '@missed' => $alert)), 'status'); 
				fclose($handle);    
			}
		}	else {
			drupal_set_message(t('There was an error uploading your file. Please contact a System administator.'), 'error');
		}
	//}// other side of the 'isset form state storage confirm else'
}
