
# Copyright (c) 2021, PostgreSQL Global Development Group

use strict;
use warnings;

use PostgreSQL::Test::Cluster;
use Test::More tests => 2;

my $node = PostgreSQL::Test::Cluster->new('main');
$node->init;
$node->start;

$node->issues_sql_like(
	[ 'vacuumdb', '-a' ],
	qr/statement: VACUUM.*statement: VACUUM/s,
	'vacuum all databases');
