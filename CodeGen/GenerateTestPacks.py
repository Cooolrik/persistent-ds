# pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
# Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

from EntitiesHelpers import *

v1_0 = Version( 
	name = "v1_0", 
	previousVersion = None, 
	items = [
		NewItem( 
			name = "TestItemA",
			variables = [ 
				Variable("string", "Name"),
				Variable("string", "OptionalText", optional = True ) 
				] 
			),
		NewEntity( 
			name = "TestEntityA", 
			dependencies = [ 
				Dependency( "ItemTable", include_in_header = True ),
				Dependency( "TestItemA", include_in_header = True ) 
				],
			templates = [ 
				Template( "test_table", template = "ItemTable", types = ["item_ref","TestItemA"] , flags = ['ZeroKeys'] ) 
				],
			variables = [ 				
				Variable( "test_table", name = "TestVariableA", optional = True ),
				Variable( "string", name = "Name"),
				Variable( "string", name = "OptionalText", optional = True ) 
				]
			),
		NewEntity( 
			name = "TestEntityC", 
			variables = [ 
				Variable("uint", "Age") 
				]
			),			
		]
	) 

v1_1 = Version( 
	name = "v1_1", 
	previousVersion = v1_0, 
	items = [
		IdenticalItem( 
			name = "TestItemA" 
			),
		IdenticalEntity( 
			name = "TestEntityA" 
			),
		NewEntity( 
			name = "TestEntityB", 
			variables = [ 
				Variable("string", "Name") 
				]
			),
		ModifiedEntity( 			
			name = "TestEntityC",
			variables = [ 
				Variable("uint", "Age"),
				Variable("string", "NameC") 
				],
			mappings = [
				SameVariable("Age"),
				NewVariable("NameC")
				]			
			),
		]
	) 

v1_2 = Version( 
	name = "v1_2", 
	previousVersion = v1_1, 
	items = [
		IdenticalItem( 
			name = "TestItemA" 
			),
		IdenticalEntity( 
			name = "TestEntityA" 
			),
		ModifiedEntity( 
			name = "TestEntityB", 
			variables = [ 
				Variable("string", "Name2") 
				],
			mappings = [ 
				RenamedVariable("Name2", "Name") 
				]
			),
		DeletedEntity(
			name = "TestEntityC" 
			),
		]
	) 

TestPackA = Package( "TestPackA", 
	path = "../Tests/TestPackA", 
	versions = [  
		v1_0,
		v1_1,
		v1_2,
		] 
	)

hlp.run_module('PackageGenerator.PackageGenerator', TestPackA, "Latest" )


