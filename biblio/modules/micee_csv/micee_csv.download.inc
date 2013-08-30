<?php
/*
 * @file micee_csv.module
 *
 */
 
function micee_csv_print_schema($node){
  $text = "";
	
	foreach ($node as $key => $value) {
		$pattern = "/micee[0-9_]*/i";
		if (preg_match($pattern, $key)){
			$sub_pattern = "/,/";
			if (preg_match($sub_pattern, $value)){
				$value = preg_replace('/"/', '""', $value);
				$value = "\"".$value."\"";
			}else{
				NULL;
			}
			$text .= $key.",".$value."\n";
		}else{
			NULL;
		}
	}
	
	$output = array(
    '#type' => 'markup',
    '#markup' => $text,
  );
	
	$bong = 'micee_node_'.$node->nid.'.csv'; // xml,pdf,doc etc as you needed
	$uri = 'public://';
	$file = file_save_data($text, 'public://' .$bong, $replace = FILE_EXISTS_REPLACE);
	$bing = file_create_url($uri);
  drupal_goto($bing.$bong);
	
}
