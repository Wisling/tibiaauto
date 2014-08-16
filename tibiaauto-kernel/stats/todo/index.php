<?php

/************************************************************************/
/* PHP-NUKE: Web Portal System                                          */
/* ===========================                                          */
/*                                                                      */
/* Copyright (c) 2005 by Francisco Burzi                                */
/* http://phpnuke.org                                                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation; either version 2 of the License.       */
/************************************************************************/

if (!defined('MODULE_FILE')) {
	die ("You can't access this file directly...");
}

require_once("mainfile.php");
$module_name = basename(dirname(__FILE__));
get_lang($module_name);

$pagetitle = "- $module_name";

function emitCreatureList()
{
	global $db;	
	global $module_name;

	$result = $db->sql_query("SELECT count(*) as c FROM stats_loot_processed WHERE pos=-1");
	$row = $db->sql_fetchrow($result);
	$totalCreaturesQty = intval($row['c']);

	echo "<center><font class=\"option\">Creatures statistics (deaths: $totalCreaturesQty)</font></center><br>"
	."<table border=1 width=\"100%\" cellpadding=\"4\" cellspacing=\"0\" border=\"0\">";
	echo "<tr>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Creature</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Registered deaths</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Looted items</b></font></td>";
	echo "</tr>";



	$result = $db->sql_query("SELECT name,count(*) as c,(SELECT count(distinct objectId) FROM stats_loot_processed WHERE name=ext.name AND pos!=-1) as objectCount FROM stats_loot_processed ext WHERE pos=-1 GROUP BY name ORDER BY name ASC");
	while ($row = $db->sql_fetchrow($result)) {
		$creatureCount = intval($row['c']);
		$creatureName = stripslashes($row['name']);
		$creatureNameUrl = urlencode($creatureName);
		$creatureObjectCount = intval($row['objectCount']);
		
		echo "<tr>";
		echo "<td><a href=modules.php?name=Loot+statistics&amp;statType=creature&amp;paramCreatureName=$creatureNameUrl>$creatureName</a></td>";
		echo "<td>$creatureCount</td>";
		echo "<td>$creatureObjectCount</td>";
		echo "</tr>";
	}
	echo "</table>";	
} // emitCreatureList

function emitCreatureDetails($creatureName)
{
	global $db;

	$result = $db->sql_query("SELECT count(*) AS c from stats_loot_processed WHERE pos=-1 AND name='$creatureName'");
	$row = $db->sql_fetchrow($result);
	$creatureCount = intval($row['c']);
	$imgCreatureName = str_replace(" ","",strtolower($creatureName));

	echo "<br><br><center><font class=\"option\"><img border=0 src=\"http://static.tibia.com/IMAGES/library/$imgCreatureName.gif\"/><br>$creatureName<br>(deaths: $creatureCount)</font></center><br>"
	."<table border=1 width=\"100%\" cellpadding=\"4\" cellspacing=\"0\" border=\"0\">";
	echo "<tr>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Code</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Name</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Seen times</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>In % corpses</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Avg. qty</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Eff. qty</b></font></td>";
	echo "</tr>";



	$result= $db->sql_query("SELECT objectId,(SELECT name FROM stats_data_items WHERE code=foo.objectId) AS objectName,c,qty from (SELECT objectId,count(*) AS c,avg(quantity) AS qty FROM stats_loot_processed WHERE objectId!=0 and pos!=-1 AND name='$creatureName' GROUP BY objectId) foo ORDER by c DESC");
	while ($row = $db->sql_fetchrow($result))
	{
		$objectId = intval($row['objectId']);
		$objectName = $row['objectName'];
		$objectCount = intval($row['c']);
		$objectPerc = number_format(($objectCount*100)/$creatureCount,2);
		$objectQty = number_format($row['qty'],2);
		$objectEffQty = number_format($objectPerc/100*$objectQty,3);
		echo "<tr>";
		echo "<td><a href=modules.php?name=Loot+statistics&amp;statType=item&amp;paramObjectId=$objectId>$objectId</a></td>";
		echo "<td><img src=\"/items/$objectId.png\"/><br>$objectName</td><td>$objectCount</td><td>$objectPerc%</td><td>$objectQty</td><td>$objectEffQty</td></tr>";
	}
	echo "</table>";	

} // emitCreatureDetails

function emitObjectDetails($objectId)
{
	global $db;

	$result = $db->sql_query("SELECT name from stats_data_items WHERE code=$objectId");
	$row = $db->sql_fetchrow($result);
	$objectName = $row['name'];
	if (strlen($objectName)==0)
		$objectName="unknown";

	echo "<br><br><center><font class=\"option\"><img src=\"/items/$objectId.png\"/><br>$objectName</font></center><br>"
	."<table border=1 width=\"100%\" cellpadding=\"4\" cellspacing=\"0\" border=\"0\">";
	echo "<tr>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Creature</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Seen times</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Reg. deaths</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>In % corpses</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Avg. qty</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Eff. qty</b></font></td>";
	echo "</tr>";



	$result= $db->sql_query("select creatureName,c,qty,creatureQty,c*100/creatureQty as perc, c/creatureQty*qty as effectiveQty from (SELECT creatureName,c,qty, (select count(*) from stats_loot_processed where name=creatureName and pos=-1) as creatureQty from (SELECT name as creatureName,count(*) AS c,avg(quantity) AS qty FROM stats_loot_processed WHERE objectId=$objectId GROUP BY name) foo ORDER by c DESC) goo order by effectiveQty desc, perc desc,qty desc");
	while ($row = $db->sql_fetchrow($result))
	{
		$creatureName = $row['creatureName'];
		$creatureNameUrl = urlencode($creatureName);
		$objectCount = intval($row['c']);
		$creatureCount = intval($row['creatureQty']);
		$objectPerc = $row['perc'];
		$objectQty = number_format($row['qty'],2);
		$objectEffQty = number_format($row['effectiveQty'],3);
		echo "<tr>";
		echo "<td><a href=modules.php?name=Loot+statistics&amp;statType=creature&amp;paramCreatureName=$creatureNameUrl>$creatureName</a></td><td>$objectCount</td><td>$creatureCount</td><td>$objectPerc%</td><td>$objectQty</td><td>$objectEffQty</td></tr>";
	}
	echo "</table>";
} // emitObjectId

function emitObjectList()
{
	
	global $db;	

	echo "<br><br><center><font class=\"option\">Known looted items</font></center><br>"
	."<table border=1 width=\"100%\" cellpadding=\"4\" cellspacing=\"0\" border=\"0\">";
	echo "<tr>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Code</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Name</b></font></td>";
	echo "<td bgcolor=\"$bgcolor2\"><font class=\"option\"><b>Seen times</b></font></td>";
	echo "</tr>";



	$result= $db->sql_query("SELECT objectId,(SELECT name FROM stats_data_items WHERE code=foo.objectId) AS objectName,c,qty from (SELECT objectId,count(*) AS c,avg(quantity) AS qty FROM stats_loot_processed WHERE objectId!=0 and pos!=-1 GROUP BY objectId) foo ORDER by objectName");
	while ($row = $db->sql_fetchrow($result))
	{
		$objectId = intval($row['objectId']);
		$objectName = $row['objectName'];
		$objectCount = intval($row['c']);
		$objectPerc = number_format(($objectCount*100)/$creatureCount,2);
		$objectQty = intval($row['qty']);
		echo "<tr>";
		echo "<td><a href=modules.php?name=Loot+statistics&amp;statType=item&amp;paramObjectId=$objectId>$objectId</a></td>";
		echo "<td><img src=\"/items/$objectId.png\"/> $objectName</td><td>$objectCount</td></tr>";
	}
	echo "</table>";	
}



function emitAd()
{
	echo "<script type=\"text/javascript\"><!--\n";
	echo "google_ad_client = \"pub-7886103199485450\";\n";
	echo "google_ad_width = 468;\n";
	echo "google_ad_height = 60;\n";
	echo "google_ad_format = \"468x60_as\";\n";
	echo "google_ad_type = \"text_image\";\n";
	echo "google_ad_channel =\"3694529527\";\n";
	echo "google_color_border = \"CCCCCC\";\n";
	echo "google_color_bg = \"FFFFFF\";\n";
	echo "google_color_link = \"000000\";\n";
	echo "google_color_url = \"666666\";\n";
	echo "google_color_text = \"333333\";\n";
	echo "//--></script>\n";
	echo "<script type=\"text/javascript\"\n";
	echo "  src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">\n";
	echo "</script>\n";
} // emitAd

global $currentlang, $multilingual;
if ($multilingual == 1) {
	$querylang = "WHERE flanguage='$currentlang'";
} else {
	$querylang = "";
}
include("header.php");
OpenTable();


if ($statType=="creature")
{
	if (strlen($paramCreatureName))
	{
		emitCreatureDetails(addslashes($paramCreatureName));
	} else {
		emitCreatureList();
	}
}

if ($statType=="item")
{
	if (strlen($paramObjectId))
	{
		emitObjectDetails(addslashes($paramObjectId));
	} else {
		emitObjectList();
	}
}

emitAd();

 
CloseTable();
include("footer.php");

?>
