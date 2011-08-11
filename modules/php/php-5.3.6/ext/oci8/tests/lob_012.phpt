--TEST--
oci_lob_export() 
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php
	
require dirname(__FILE__).'/connect.inc';
require dirname(__FILE__).'/create_table.inc';

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (blob)
                      VALUES (empty_blob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$statement = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($statement, OCI_DEFAULT);

$blob;

var_dump($blob->write("test string is here\nnew string"));

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

$row = oci_fetch_array($s);

var_dump($row[0]->export(dirname(__FILE__)."/lob_012.tmp", 3, 10));

var_dump(file_get_contents(dirname(__FILE__)."/lob_012.tmp"));

@unlink(dirname(__FILE__)."/lob_012.tmp");

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECT--
int(30)
bool(true)
string(10) "t string i"
Done
