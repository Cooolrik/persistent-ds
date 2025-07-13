# pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
# Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

from EntitiesHelpers import *

v1_0 = Version( 
	name = "v1_0", 
	previousVersion = None, 
	mods = [
		AddItem( 
			name = "TestItemA",
			variables = [ 
				Variable("string", "Name"),
				Variable("string", "OptionalText", optional = True ) 
			] 
		),
		AddEntity( 
			name = "TestEntityA", 
			dependencies = [ 
				Dependency( "ItemTable", include_in_header = True ),
				Dependency( "TestItemA", include_in_header = True ) 
			],
			templates = [ 
				Template( "test_table", template = "ItemTable", types = ["item_ref","TestItemA"] , flags = ['zero_keys'] ) 
			],
			variables = [ 				
				Variable( "test_table", name = "TestVariableA", optional = True, storageName="TstVarA" ),
				Variable( "string", name = "Name"),
				Variable( "string", name = "OptionalText", optional = True, storageName="OptTxt" ) 
			]
		),
		AddEntity( 
			name = "TestEntityC", 
			variables = [ 
				Variable("u32", "Age") 
			]
		),			
	]
) 

v1_1 = Version( 
	name = "v1_1",
	previousVersion = v1_0,
	mods = [
		AddEntity( 
			name = "TestEntityB", 
			variables = [ 
				Variable("string", "Name") 
			]
		),
		ModifyEntity(
			name = "TestEntityC",
			addVariables=[
				Variable("string", "NameC") 
			],
		),
	]
)

v1_2 = Version( 
	name = "v1_2",
	previousVersion = v1_1,
	mods = [
		DeleteEntity(
			name = "TestEntityC"
		),
		ModifyEntity(
			name = "TestEntityB",
			renameVariables=[
				("Name","Name2")
			]
		)
	]
)


TestPackA = Package( "TestPackA", 
	path = "../Tests/TestPackA", 
	versions = [  
		v1_0,
		v1_1,
		v1_2
		] 
	)

print(TestPackA)

hlp.run_module('PackageGenerator.PackageGenerator', TestPackA, "Latest" )


