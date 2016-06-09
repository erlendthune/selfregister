<?php
//This is where the form is submitted when the user pushes the register user button.
//You do normally not have to change this.
$formaction = "createuser.php";

//Outputs debug information on screen if set to TRUE.
$verbose = FALSE;

//Used to inform the user where she can get help.
$supportlink = "<a target='_blank' href='mailto:programmering@iktsenteret.no'>programmering@iktsenteret.no</a>";

//This is were the Canvas REST API server is. The user will be registered there.
$site = "https://kurs.iktsenteret.no";

//The name of the study. Used in various messages on screen.
$study = "ProgrammeringsMOOC";

//The name of the FEIDE service as described at innsyn.feide.no->Tjenester
$feidetjeneste = "kurs.iktsenteret.no";

//This is the access key used. It should be kept secret, i.e. it should not be possible to read
//the contents of this file by external web users.
//This guide explains how to create an access key: 
//https://guides.instructure.com/m/4214/l/40399-how-do-i-obtain-an-api-access-token-for-an-account
$access_key = "";

//A list of the courses the user should be enrolled into.
$courses = array(5,15,16,17,18,22);

//A list of the groups the user should be enrolled into.
$groups = array(2,5);

//The user will be enrolled in this account.
$account_id=1;
?>