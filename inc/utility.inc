<?php
/*
File   :utility.inc
Author :Erlend Thune
Email  :et@iktsenteret.no
Date   :26.11.2015
Purpose:Utility functions for self registering users in the instructure Canvas platform.
Todo   :Current version tested with feide addresses only.
        Refactor success and error messages.
        Improve error handling. 
*/

///////////////////
///////////////////
//Public functions
///////////////////
///////////////////
////////////////////////////////////////////////////////////////////////////
//Name   : handleRequest
//Purpose: If called with parameters, checks the parameters and registers the user
//         specified. Otherwise just prints the register user form.
////////////////////////////////////////////////////////////////////////////
function handleRequest()
{
	printHtmlHeader();
	$registerType = getParameter("registerType");
	$usersname = getParameter("usersname");
	$feideid = getParameter("feideid");
	$epost = getParameter("epost");

	$bSuccess = checkFieldsPresent($usersname, $feideid, $epost, $registerType);
	if($bSuccess)
	{
		$bSuccess = checkFieldsValid($usersname, $feideid, $epost, $registerType);
	}
	if($bSuccess)
	{
		addUser($usersname, $feideid, $epost, $registerType);
	}
	else
	{
		printRegisterFormBody($usersname, $feideid, $epost, $registerType);
	}
	printHtmlFooter();
}

////////////////////////////////////////////////////////////////////////////
//Name   : printRegisterForm
//Purpose: Gets the registerType parameter and calls printRegisterFormBody
//corresponding function.
////////////////////////////////////////////////////////////////////////////
function printRegisterForm()
{	
	printHtmlHeader();
	mydbg("printRegisterForm BEGIN");
	$registerType = getParameter("registerType");
	mydbg("Registertype:$registerType");

	$usersname = "";
	$feideid = "";
	$epost = "";
	printRegisterFormBody($usersname, $feideid, $epost, $registerType);

	mydbg("printRegisterForm END");
	printHtmlFooter();
}

///////////////////
///////////////////
//Private functions
///////////////////
///////////////////


////////////////////////////////////////////////////////////////////////////
//Name   : checkFieldsPresent
//Purpose: Checks if neccessary fields are set, depending on registerType.
////////////////////////////////////////////////////////////////////////////
function checkFieldsPresent($usersname, $feideid, $epost, $registerType)
{
    $bSuccess = true;
   	if($usersname == "")
    {
    	Error("Vennligst oppgi et navn."); 
    	$bSuccess = false;
	}

	if($registerType == "feide")
	{
		if($feideid == "")
	    {
    		Error("Vennligst oppgi en FEIDE-id."); 
    		$bSuccess = false;
		}
	}

	if($epost == "")
    {
    	Error("Vennligst oppgi en epostadresse."); 
    	$bSuccess = false;
    }
    
	return $bSuccess;
}

////////////////////////////////////////////////////////////////////////////
//Name   : checkFieldsValid
//Purpose: Checks the validity of the fields, depending on registerType.
////////////////////////////////////////////////////////////////////////////
function checkFieldsValid($usersname, $feideid, $epost, $registerType)
{
	$bSuccess = true;
	if($registerType == "feide")
	{
		if (!filter_var($feideid, FILTER_VALIDATE_EMAIL)) 
		{
		  Error("Vennligst oppgi en gyldig FEIDE-id.");
		  $bSuccess = false;
		}
    }
    
	if (!filter_var($epost, FILTER_VALIDATE_EMAIL)) 
	{
	  Error("Vennligst oppgi en gyldig epostadresse.");
	  $bSuccess = false;
	}
	return $bSuccess;
}

////////////////////////////////////////////////////////////////////////////
//Name   : addUser
//Purpose: Checkes if the user exists. If it does not exists the user is created.
//         If the user existed, it is enrolled in the courses and added to the groups
//         specified in vars.inc
//         If either of these fails, an error message is printed.
////////////////////////////////////////////////////////////////////////////
function addUser($usersname, $feideid, $epost, $registerType)
{
      $bUserExisted = FALSE;
      $user_id = 0;
      if($registerType == "canvas")
      {
   	  	$user_id = userExist($epost);
      }
      else
      {
   	  	$user_id = userExist($feideid);
   	  }
   	  if(!$user_id)
   	  {
   	  	  mydbg($registerType);
   	  	  if($registerType == "canvas")
   	  	  {
   	  	  	mydbg("Calling createUser");
   	  	  	$user_id = createUser($usersname, $epost);
   	  	  }
   	  	  else
   	  	  {
   	  	  	mydbg("Calling createFeideUser");
   	  	  	$user_id = createFeideUser($usersname, $feideid, $epost);
   	  	  }
	  }
	  else
	  {
	     $bUserExisted = TRUE;
	  }
	  if($user_id || $bUserExisted)
	  {
	  
	 	  if(enrollUser($user_id) && AddUserToGroups($user_id))
	 	  {
			printSuccessMessage($bUserExisted, $registerType);
		  }
		  else
		  {
			printErrorMessage($bUserExisted,$registerType);
		  }
	  }
	  else
	  {
	  	Error("Kunne ikke opprette bruker.");
		printErrorMessage($bUserExisted, $registerType);
	  }
    printHtmlFooter();
}


////////////////////////////////////////////////////////////////////////////
//Name   : printRegisterFormBody
//Purpose: Prints the register form depending on registerType.
////////////////////////////////////////////////////////////////////////////
function printRegisterFormBody($usersname, $feideid, $epost, $registerType)
{
	global $formaction;

	$stuff = <<<EOT
<form class="form-inline" role="form" method="post" action="$formaction?registerType=$registerType">
	<div class="form-group">
	  <input type="hidden" name="registertype" value="$registerType"/>
  	  <label for="feideid">Navn:</label>
EOT;
	if($usersname == "")
	{
		$stuff .= <<< EOT
	  <input type="text" size="50" class="form-control" id="usersname" name="usersname" placeholder="Kari Nordmann">
EOT;
	}
	else
	{
		$stuff .= <<< EOT
	  <input type="text" size="50" class="form-control" id="usersname" name="usersname" value="$usersname">
EOT;
	}

	if($registerType == "feide")
	{
		//Feideid felt
		$stuff .= <<< EOT
	 <span class="help-block">Dette er navnet du får på diplomet så skriv det riktig!</span>
	</div>
	<div class="form-group">
  	  <label for="feideid">Feide ID:</label>
EOT;
	
		if($feideid == "")
		{
			$stuff .= <<< EOT
	  <input type="text" size="50" class="form-control" id="feideid" name="feideid" placeholder="feideid@organisasjon.no">
EOT;
		}
		else
		{
		$stuff .= <<< EOT
	  <input type="text" size="50" class="form-control" id="feideid" name="feideid" value="$feideid">
EOT;
		}
		$stuff .= <<< EOT
	 <span class="help-block">Gå inn på <a href="http://innsyn.feide.no" target="_blank">innsyn.feide.no</a>&nbsp;</strong>og finn din <strong>personlige Id </strong>hos din organisasjon</span>
	</div>
EOT;
	} //End if registertype == Feide

	//Epost felt
	$stuff .= <<< EOT
	<div class="form-group">
  	<label for="epost">Epost:</label>
EOT;
	if($epost == "")
	{
		$stuff .= <<< EOT
	  <input type="text" size="50" class="form-control" id="epost" name="epost" placeholder="din@epost.no">
EOT;
	}
	else
	{
		$stuff .= <<< EOT
	  <input type="text" size="50" class="form-control" id="epost" name="epost" value="$epost">
EOT;
	}

	$stuff .= <<< EOT
	 <span class="help-block">Digitale utmerkelser blir sendt til denne eposten. Skriv den inn riktig med en gang, så slipper du trøbbel!</span>
	</div>
	  <input class="btn btn-primary" type="submit" value="Registrer" id="registrer">
</form>
Registreringsprosessen kan ta inntil 10 sekunder, så vær tålmodig.
EOT;
	echo $stuff;
}


////////////////////////////////////////////////////////////////////////////
//Name   : printSuccessMessage
//Purpose: Called when registering the user was a success. Prints a success message.
//         The messge printed depends on whether the user existed and on registerType.
////////////////////////////////////////////////////////////////////////////
function printSuccessMessage($bUserExisted,$registerType)
{
	global $site;
	global $supportlink;
	global $study;
	global $feidetjeneste;
	$stuff = "";
	if($bUserExisted)
	{
		if($registerType == "canvas")
		{
			$stuff = <<<EOT
<p style="font-weight: bold;font-size: 150%;">Kontoen din eksisterte allerede, og vi la deg til $study. 
Du kan logge inn ved å gå inn på <a target="_blank" href="$site">$site</a> og velge <b>Vanlig innlogging</b>. 
Der oppgir du ditt brukernavn og passord. Dersom du har glemt ditt passord, kan du velge å sette et nytt på den siden.</p>
<pstyle="font-weight: bold;font-size: 150%;">Ta kontakt med $supportlink dersom du ikke lykkes med dette.</p>
EOT;
		}
		else
		{
			$stuff = <<<EOT
<p style="font-weight: bold;font-size: 150%;">Kontoen din eksisterte allerede, og vi la deg til $study. 
Du kan logge inn ved å gå inn på <a target="_blank" href="$site">$site</a>. 
Dersom du ikke finner din Feide-organisasjon i listen må du kontakte IT-avdelingen 
din for å få de til å åpne for $feidetjeneste.</p>
EOT;
		}
	}
	else
	{
		if($registerType == "canvas")
		{
			$stuff = <<<EOT
		<p style="font-weight: bold;font-size: 150%;">
		Din konto ble opprettet. Sjekk eposten din og fullfør registreringen din 
		ved å følge lenken og opprette et passord. Husk å sjekke søppelposten dersom du ikke har mottatt
		noen mail fra $study.
		</p>
		<p style="font-weight: bold;font-size: 150%;">
		Når du har opprettet et passord vil du bli logget inn.
		</p>
		<p style="font-weight: bold;font-size: 150%;">
		Ta kontakt med $supportlink dersom du ikke lykkes med dette.
		</p>
EOT;
		}
		else
		{
			$stuff = <<<EOT
		<p style="font-weight: bold;font-size: 150%;">
		Din konto er nå opprettet og du er meldt på $study. 
		Du kan logge inn ved å gå inn på <a target="_blank" href="$site">$site</a>.
		Dersom du ikke finner din Feide-organisasjon i listen må du kontakte IT-avdelingen din 
		for å få de til å åpne for $feidetjeneste.
		</p>
EOT;
		}
	}
	echo $stuff;
}

/*
                      <p style="color: #fff; padding-left: 68px;">
                            Registrer deg på MatematikkMOOC etterutdanning ved å oppgi din epostadresse nedenfor. Trykk deretter på "Registrer" knappen.
                            
                      </p>
*/

////////////////////////////////////////////////////////////////////////////
//Name   : printErrorMessage
//Purpose: Called when registering the user failed. Prints an error message.
//         The message printed depends on whether the user existed and on registerType.
////////////////////////////////////////////////////////////////////////////
function printErrorMessage($bUserExisted,$registerType)
{
	global $supportlink;
    $stuff = "";
    
	if($bUserExisted)
	{
	    $stuff .= <<<EOT
	<p style="font-weight: bold;font-size: 150%;">
    Brukeren din eksisterte og vi prøvde å legge deg til kursene. 
    </p>
EOT;
	}
    $stuff .= <<<EOT
<p style="font-weight: bold;font-size: 150%;">Noe gikk galt i registreringsprosessen, og vi 
klarte ikke å registrere deg på kursene. Ta kontakt med $supportlink.
</p>
EOT;
	echo $stuff;
}
////////////////////////////////////////////////////////////////////////////
//Name   : getParameter
//Purpose: Returns specified GET parameter from http query.
////////////////////////////////////////////////////////////////////////////
function getParameter($pname)
{
	$p = "";
    if(IsSet($_GET[$pname]))
    {
      $p = $_GET[$pname];
    }
    else if(IsSet($_POST[$pname]))
    {
      $p = $_POST[$pname];
    }
	return $p;
}


////////////////////////////////////////////////////////////////////////////
//Name   : mydbg
//Purpose: Prints debug message if verbose is set to TRUE in vars.inc
////////////////////////////////////////////////////////////////////////////
function mydbg($s)
{
	global $verbose;
	if($verbose)
	{
		echo "<p>$s</p>";
	}
}

////////////////////////////////////////////////////////////////////////////
//Name   : myvardump
//Purpose: Dumps a variable to the screen if verbose is set to TRUE in vars.inc
////////////////////////////////////////////////////////////////////////////
function myvardump($v)
{
	global $verbose;
	if($verbose)
	{
		var_dump($v);
	}
}

////////////////////////////////////////////////////////////////////////////
//Name   : HandleError
//Purpose: Prints the specified url and dumps the result variable specified.
////////////////////////////////////////////////////////////////////////////
function HandleError($result, $url)
{
	echo "<p>$url</p>";
	var_dump($result);
}

////////////////////////////////////////////////////////////////////////////
//Name   : Error
//Purpose: Prints an error message.
////////////////////////////////////////////////////////////////////////////
function Error($s)
{
	echo "<p style='color:red'>$s</p>";
}

////////////////////////////////////////////////////////////////////////////
//Name   : urlify
//Purpose: Used by mycurl to format the POST data.
////////////////////////////////////////////////////////////////////////////
function urlify($fields)
{
	//url-ify the data for the POST
	$fields_string = "";
	foreach($fields as $key=>$value) 
	{ 
		$fields_string .= $key.'='.$value.'&'; 
	}
	rtrim($fields_string, '&');
	return $fields_string;
}


////////////////////////////////////////////////////////////////////////////
//Name   : mycurl
//Purpose: Calls the Canvas server as specified by the url, fields.
//         If $post is set to true, a POST message is sent. Otherwise GET is used.
////////////////////////////////////////////////////////////////////////////
function mycurl($url, $fields, $post)
{
	global $verbose;
	global $access_key;
	
	$fields_string = urlify($fields); 
	$curl = curl_init();

	// CURLOPT_VERBOSE: TRUE to output verbose information. Writes output to STDERR, 
	// or the file specified using CURLOPT_STDERR.
	curl_setopt($curl, CURLOPT_VERBOSE, true);

	if($verbose)
	{
		$verbose = fopen('php://temp', 'w+');
		curl_setopt($curl, CURLOPT_STDERR, $verbose);	    
	}

	curl_setopt($curl, CURLOPT_URL, $url); 
	if($post)
	{
		curl_setopt($curl, CURLOPT_POST, count($fields));
		curl_setopt($curl, CURLOPT_POSTFIELDS, $fields_string);
	}
	curl_setopt($curl, CURLOPT_HTTPHEADER, array('Authorization: Bearer ' . $access_key));
	curl_setopt($curl, CURLOPT_RETURNTRANSFER,TRUE);
	

	mydbg("<p>Executing curl start</p>");

	$result = curl_exec($curl);
	
	if ($result === FALSE) 
	{
		printf("cUrl error (#%d): %s<br>\n", curl_errno($curl),
			   htmlspecialchars(curl_error($curl)));
	}
	mydbg("<p>Curl executed</p>");

	if($verbose)
	{
		rewind($verbose);
		$verboseLog = stream_get_contents($verbose);

		echo "Verbose information:\n<pre>", htmlspecialchars($verboseLog), "</pre>\n";        

		$version = curl_version();
		extract(curl_getinfo($curl));
		$metrics = <<<EOD
URL....: $url
Code...: $http_code ($redirect_count redirect(s) in $redirect_time secs)
Content: $content_type Size: $download_content_length (Own: $size_download) Filetime: $filetime
Time...: $total_time Start @ $starttransfer_time (DNS: $namelookup_time Connect: $connect_time Request: $pretransfer_time)
Speed..: Down: $speed_download (avg.) Up: $speed_upload (avg.)
Curl...: v{$version['version']}
EOD;
	}

	curl_close($curl);
	return $result;
}

////////////////////////////////////////////////////////////////////////////
//Name   : getUserId
//Purpose: Gets the user id from the specified array map.
////////////////////////////////////////////////////////////////////////////
function getUserId($arr)
{
	$user_id = 0;
	foreach($arr as $key=>$value) 
	{
		if($key == "id")
		{
			$user_id = $value;
		}
		//var_dump($key);
		//var_dump($value); 
	}
	return $user_id;
}

////////////////////////////////////////////////////////////////////////////
//Name   : userExist
//Purpose: Checks if a user with sis_id $epost exists.
////////////////////////////////////////////////////////////////////////////
function userExist($epost)
{
	global $site;
	global $account_id;
	$url = $site . "/api/v1/users/sis_user_id:" . $epost;
	$user_id = 0;
	
	mydbg("Searching for ". $epost);
	mydbg($url);

	$post = 0;
	$fields = array();
	$result = mycurl($url, $fields, $post);
	if (!($result === FALSE)) 
	{
		mydbg("Call succeeded");
		$content = json_decode($result,true);
		//var_dump($content);

		foreach($content as $key=>$value) 
		{
			if($key == "errors")
			{
				mydbg("User did not exist.");
				break;
			}
			else 
			{
				mydbg("Key:");
				myvardump($key);
				mydbg("Value:");
				myvardump($value); 
				mydbg("Type:" . gettype($key));
				if($key == "id")
				{
					myvardump($value);
					if(sizeof($value))
					{
						mydbg("User exists");	
						$user_id = $value;
						mydbg("user id=" . $user_id);
						break;
					}
				}
				else
				{
					mydbg("User did not exist.");
				}
			}
		}
	}
	else
	{
		mydbg("Call failed");
	}
	return $user_id;
}

////////////////////////////////////////////////////////////////////////////
//Name   : createUserEx
//Purpose: Sends a request to the Canvas server to create a user with the specified fields.
//Viktig å ha med variabelen om at man aksepterer terms of use, hvis ikke får man denne feilmeldingen:
//{"errors":{"user":{"terms_of_use":[{"attribute":"terms_of_use","type":"accepted","message":"accepted"}]},"pseudonym":{},"observee":{}}}
//Med mindre man er site admin, da får man lov til alt.
function createUserEx($fields)
{
	global $site;
	global $account_id;

	mydbg("Calling create user");
//	var_dump($fields);

	$url = $site . "/api/v1/accounts/" . $account_id . "/users";

	$post = 1;
	$result = mycurl($url, $fields, $post);
	
	$user_id = 0;

	if (!($result === FALSE)) 
	{
		$content = json_decode($result,true);
//		var_dump($content);
		foreach($content as $key=>$value) 
		{
			if($key == "errors")
			{
				mydbg("create user failed!" . $key);
				HandleError($result, $url);
			}
			else 
			{
				if($key == "id")
				{
					$user_id = $value;
				}

//				var_dump($key);
//				var_dump($value); 
			}
		}
	}
	else
	{
		mydbg("Failed to create user");
	}
	return $user_id;
}

////////////////////////////////////////////////////////////////////////////
//Name   : createUser
//Purpose: Call this function to create a traditional Canvas user.
////////////////////////////////////////////////////////////////////////////
function createUser($usersname, $epost)
{
	mydbg("createUser BEGIN");
	mydbg("usersname:$usersname");
	mydbg("epost:$epost");

	$fields = array( 
		'user[name]' => $usersname,
		'pseudonym[unique_id]' => $epost,
		'pseudonym[sis_user_id]' => $epost,
		'user[terms_of_use]' => 1,
		'pseudonym[send_confirmation]' => 1,
		'enable_sis_reactivation' => 1
	); 
	mydbg("createUser END");
	return createUserEx($fields);
}

////////////////////////////////////////////////////////////////////////////
//Name   : createFeideUser
//Purpose: Creates a Feide user with a corresponding email.
//         Note that the existence of the email address is not checked.
//         This is because the confirmation message would contain a link
//         that would not lead to the Feide login screen.
////////////////////////////////////////////////////////////////////////////
function createFeideUser($usersname, $feideid, $epost)
{
	$fields = array( 
		'user[name]' => $usersname,
		'pseudonym[unique_id]' => $feideid,
		'pseudonym[sis_user_id]' => $feideid,
		'user[skip_registration]' => 1,
		'user[terms_of_use]' => 1,
		'pseudonym[send_confirmation]' => 0,
		'communication_channel[type]' => "email",
		'communication_channel[address]' => $epost,
		'communication_channel[skip_confirmation]' => 1,
		'enable_sis_reactivation' => 1
	); 
	myvardump($fields);
	return createUserEx($fields);
}

////////////////////////////////////////////////////////////////////////////
//Name   : addCommunicationChannel
//Purpose: Adds a communication channel.
//         This function is not used at the moment.
////////////////////////////////////////////////////////////////////////////
///api/v1/users/:user_id/communication_channels
function addCommunicationChannel($user_id,$epost)
{
	global $site;
	global $account_id;
	$fields = array( 
		'communication_channel[type]' => "email",
		'communication_channel[address]' => $epost,
		'communication_channel[skip_confirmation]' => 0
	); 

	mydbg("Calling create user");
//	var_dump($fields);

	$url = $site . "/api/v1/users/" . $user_id . "/communication_channels";

	$post = 1;
	$result = mycurl($url, $fields, $post);
	
	if (!($result === FALSE)) 
	{
		$content = json_decode($result,true);
//		var_dump($content);
		foreach($content as $key=>$value) 
		{
			if($key == "errors")
			{
				Error("Epost: " .$epost);
				mydbg("create user failed!" . $key);
				HandleError($result, $url);
			}
			else 
			{
				if($key == "id")
				{
					$user_id = $value;
				}

//				var_dump($key);
//				var_dump($value); 
			}
		}
	}
	else
	{
		mydbg("Failed to create user");
	}
	return $user_id;

}

////////////////////////////////////////////////////////////////////////////
//Name   : addUserToGroup
//Purpose: Adds a user to a group.
////////////////////////////////////////////////////////////////////////////
function addUserToGroup($user_id,$gid)
{
	global $site;
	global $verbose;
	$ret_value = 0;

	$url = $site . "/api/v1/groups/" . $gid . "/memberships";
	$fields = array( 
		user_id => $user_id
	); 
	$post = 1;
	$result = mycurl($url, $fields, $post);
	if (!($result === FALSE)) 
	{
		$content = json_decode($result,true);

		foreach($content as $key=>$value) 
		{
			mydbg(gettype($key));
			mydbg(gettype($value));
			if($key == "errors")
			{
			    $ret_value = 0;
			    Error("User:" . $user_id);
				HandleError($result, $url);
			}
			else 
			{
				$ret_value = 1;
//				var_dump($key);
//				var_dump($value); 
			}
		}
	}
	return $ret_value;
}

////////////////////////////////////////////////////////////////////////////
//Name   : addUserToGroups
//Purpose: Adds a user to the groups specified in vars.inc
////////////////////////////////////////////////////////////////////////////
function addUserToGroups($user_id)
{
	global $groups;
	$ret_value = 0;
	$noOfGroups = 0;
	if($user_id)
	{
		$noOfGroups = count($groups);
		foreach ($groups as $group) 
		{
			$ret_value += addUserToGroup($user_id,$group);
		}
	}
	
	$bSuccess = FALSE;
	if($ret_value == $noOfGroups)
	{
		$bSuccess = TRUE;
	}
	return $bSuccess;
}

////////////////////////////////////////////////////////////////////////////
//Name   : enrollUserInCourse
//Purpose: Enroll a user in a course.
////////////////////////////////////////////////////////////////////////////
function enrollUserInCourse($user_id,$cid)
{
	global $site;
	global $verbose;
	$ret_value = 1;

	$url = $site . "/api/v1/courses/" . $cid . "/enrollments";
	$fields = array( 
		'enrollment[user_id]' => $user_id, 
		'enrollment[type]' => "StudentEnrollment",
		'enrollment[enrollment_state]' => "active"
	); 
	$post = 1;
	$result = mycurl($url, $fields, $post);
	if (!($result === FALSE)) 
	{
		$content = json_decode($result,true);

		foreach($content as $key=>$value) 
		{
			mydbg(gettype($key));
			mydbg(gettype($value));
			if($key == "errors")
			{
			    $ret_value = 0;
			    Error("User:" . $user_id);
				HandleError($result, $url);
			}
			else 
			{
//				var_dump($key);
//				var_dump($value); 
			}
		}
	}
	return $ret_value;
}

////////////////////////////////////////////////////////////////////////////
//Name   : enrollUser
//Purpose: Enroll a user in the courses specified in vars.inc
////////////////////////////////////////////////////////////////////////////
function enrollUser($user_id)
{
	global $courses;
	$ret_value = 0;
	$noOfCourses = 0;
	if($user_id)
	{
		$noOfCourses = count($courses);
		foreach ($courses as $course) 
		{
			$ret_value += enrollUserInCourse($user_id,$course);
		}
	}
	
	$bSuccess = FALSE;
	if($ret_value == $noOfCourses)
	{
		$bSuccess = TRUE;
	}
	return $bSuccess;
}

////////////////////////////////////////////////////////////////////////////
//Name   : printHtmlHeader
//Purpose: Prints the html header.
////////////////////////////////////////////////////////////////////////////
function printHtmlHeader()
{
	global $study;
	$stuff = <<<EOT
<!doctype html>
<html lang="no">
<head>
	<meta charset="UTF-8">
<!-- Latest compiled and minified CSS -->
<link rel="stylesheet" href="http://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css">

<!-- jQuery library -->
<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>

<!-- Latest compiled JavaScript -->
<script src="http://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js"></script>	


  <title>$study</title>
  <style>
body {
    background-color: #ffffff;
}
</style>

</head>
<body style="background-color:#ffffff ! important;">
EOT;
	echo $stuff;
}


////////////////////////////////////////////////////////////////////////////
//Name   : printHtmlFooter
//Purpose: Prints the html footer
////////////////////////////////////////////////////////////////////////////
function printHtmlFooter()
{
	$stuff = <<<EOT
</body>
</html>
EOT;
	echo $stuff;
}


?>