CREATE OR REPLACE FUNCTION xql6(text, text)
RETURNS cstring
AS 'xql6.so', 'xql6'
LANGUAGE C STRICT;
