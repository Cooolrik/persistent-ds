# pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
# Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

from __future__ import annotations
import CodeGeneratorHelpers as hlp
import sys
import copy

# class types which are built in to pds
pds_built_in_item_types = {
	'BidirectionalMap',
	'DirectedGraph',
	'IndexedVector',
	'ItemTable',
	'Varying'
}

class Dependency:
	"""definition of a dependency item/entity in the package or a built-in type from pds"""	
	def __init__(self, name, include_in_header = False ):
		self.Name = name
		self.IncludeInHeader = include_in_header
		self.PDSType = name in pds_built_in_item_types

class Template:
	"""implementation of a template class"""
	def __init__(self, name, template, types, flags = [] ):
		self.Name = name
		self.Template = template
		self.Types = types
		self.Flags = flags

		# create declaration of template
		self.Declaration = f'using {self.Name} = pds::{self.Template}<'
		has_value = False
		for ty in self.Types:
			if has_value:
				self.Declaration += ','
			self.Declaration += ty
			has_value = True
		if len(self.Flags) > 0:
			if has_value:
				self.Declaration += ','
			has_flag = False;
			for fl in self.Flags:
				if has_flag:
					self.Declaration += '|'
				self.Declaration += 'pds::' + hlp.camel_to_snake(self.Template) + '_flags::' + fl
				has_flag = True
			has_value = True
		self.Declaration += '>;'

class Variable:
	"""a variable in the item/entity"""
	def __init__(self, type, name, optional = False, vector = False, indexed = False, storageName:str = None ):
		self.Type = type
		self.Name = name
		self.Optional = optional
		self.Vector = vector
		self.IndexedVector = indexed
		self.StorageName = storageName if storageName is not None else name
		if self.IndexedVector and not self.Vector:
			sys.exit("Variable.__init__: IndexedVector requires Vector flag to be set as well")

		# build the type string
		if self.Optional:
			if self.Vector:
				if self.IndexedVector:
					self.TypeString = f"optional_idx_vector<{self.Type}>"
				else:
					self.TypeString = f"optional_vector<{self.Type}>"
			else:
				self.TypeString = f"optional_value<{self.Type}>"
		else:
			if self.Vector:
				if self.IndexedVector:
					self.TypeString = f"idx_vector<{self.Type}>"
				else:
					self.TypeString = f"std::vector<{self.Type}>"
			else:
				self.TypeString = self.Type

		# check if this is a simple value, or a complex value (which is using one of the wrapper classes)
		self.IsSimpleValue = (not self.Optional) and (not self.Vector) and (not self.IndexedVector)
		self.IsComplexValue = not self.IsSimpleValue

		# look up BaseType and BaseVariant
		self.BaseType,self.BaseVariant = hlp.get_base_type_variant(self.Type)
		self.IsBaseType = self.BaseType is not None

		# check if this is a simple value which is a base type
		self.IsSimpleBaseType = self.BaseType and self.IsSimpleValue

class Validation:
	"""Validation type"""

class ValidateAllKeysAreInTable(Validation):
	"""validation that makes sure all keys in one table or map are keys in another table"""

	def __init__(self, tableToValidate:str, mustExistInTable:str):
		self.TableToValidate = tableToValidate
		self.MustExistInTable = mustExistInTable

	def GenerateValidationCode( self, entity, indentation ):
		# find the types of the variables
		tableType = entity.FindVariable( self.TableToValidate ).Type
		lines = []
		lines.append( f'{indentation}// Validate that all keys in {self.TableToValidate} also exist in {self.MustExistInTable}' )
		lines.append( f'{indentation}success = {tableType}::MF::ValidateAllKeysAreContainedInTable( obj.v_{self.TableToValidate} , validator , obj.v_{self.MustExistInTable} , "{self.MustExistInTable}" );' )
		lines.append( f'{indentation}if( !success )' )
		lines.append( f'{indentation}    return false;' )
		return lines

class Mapping:
	"""Base class of variable mappings between versions of items"""
	def __init__(self, *, 
			  name:str,  
			  previousName:str = None,
			  isDeleted:bool = False,
			  toPreviousCode:str = None, 
			  fromPreviousCode:str = None
			  ):
		self.Name = name
		self.PreviousName = previousName
		self.IsDeleted = isDeleted
		self.ToPreviousCode = toPreviousCode
		self.FromPreviousCode = fromPreviousCode

	def IsNew(self) -> bool:
		"""return True if this is a new variable, i.e. it does not have a previous name"""
		return self.PreviousName == None
	

class Item:
	"""item base class"""
	def __init__(
			self, *, 
			name:str, 
			variables:list[Variable] = [], 
			dependencies:list[Dependency] = [], 
			templates:list[Template] = [], 
			validations:list[Validation] = [],
			mappings:list[Mapping] = [],
			version:Version,
			previousVersion:Item,
			isEntity:bool,
			isDeleted:bool = False,
			isIdenticalToPreviousVersion:bool = False,
			isModifiedFromPreviousVersion:bool = False,
			isDeprecated:bool = False,
			):
		self.Name = name
		self.IsEntity = isEntity
		self.IsIdenticalToPreviousVersion = isIdenticalToPreviousVersion
		self.IsDeleted = isDeleted
		self.IsDeprecated = isDeprecated
		self.IsModifiedFromPreviousVersion = isModifiedFromPreviousVersion
		self.IsReleaseVersion = False
		self.Dependencies = dependencies
		self.Templates = templates
		self.Variables = variables
		self.Validations = validations
		self.Mappings = mappings
		self.Version = version
		self.PreviousVersion = previousVersion

	def GetImplementingItem(self) -> Item:
		"""return the item in a previous version which implements this item. if this is a new item, return self. if it is deleted, return None"""
		if self.IsDeleted:
			return None
		prev_version = self
		while prev_version.IsIdenticalToPreviousVersion:
			prev_version = prev_version.PreviousVersion
		return prev_version

	def GetImplementingHeaderFilePath(self) -> str:
		"""return the file path to the item in the previous version which implements this item, relative to this file path. if this is a new item, return self. if it is deleted, return None"""
		if self.IsReleaseVersion:
			return "../" + self.Name + ".h"
		if self.IsDeleted:
			return None
		prev_version = self.GetImplementingItem()
		if prev_version == None:
			return None
		return f'../{prev_version.Version.Name}/{prev_version.Version.Name}_{prev_version.Name}.h'

	def GetPathToRoot(self) -> str:
		"""return the path to the root of the package, relative to this file path"""
		if self.IsReleaseVersion:
			return "./"
		else:
			return "../" 
		
	def GetPathToPreviousVersion(self) -> str:
		"""return the path to the previous version of the item, relative to this file path"""
		if self.IsDeleted:
			return None
		if not self.IsModifiedFromPreviousVersion:
			return None
		return self.GetPathToRoot() + self.PreviousVersion.Version.Name + "/" + self.PreviousVersion.Version.Name + "_" + self.PreviousVersion.Name + ".h"

	def FindDependency( self, name ):
		"""find a dependency with the given name"""
		if self.Dependencies is None or len(self.Dependencies) == 0:
			return None
		return next( (val for val in self.Dependencies if val.Name == name), None )

	def FindTemplate( self, name ):
		"""find a template with the given name"""
		if self.Templates is None or len(self.Templates) == 0:
			return None
		return next( (val for val in self.Templates if val.Name == name), None )

	def FindVariable( self, name ):
		"""find a variable with the given name"""
		if self.Variables is None or len(self.Variables) == 0:
			return None
		return next( (val for val in self.Variables if val.Name == name), None )

	def FindMapping( self, name:str ) -> Mapping:
		"""find a mapping for the variable with the given name"""
		if self.Mappings is None or len(self.Mappings) == 0:
			return None
		return next( (val for val in self.Mappings if val.Name == name), None )

class Modification:
	"""base class for modifications to items and entities in a package version"""
	def __init__(self, *, name:str):
		self.name = name

class AddItem(Modification):
	"""add a new item to the package version"""
	def __init__(self, *, name:str, variables:list[Variable], dependencies:list[Dependency] = [], templates:list[Template] = [], validations:list[Validation] = [], isEntity:bool = False ):
		super().__init__(name=name)
		self.Dependencies = dependencies
		self.Templates = templates
		self.Variables = variables
		self.Validations = validations
		self.IsEntity = isEntity

	def Apply(self, version:Version) -> None:
		"""apply the modification to the version object, adding the item to the version"""
		if version.FindItem(self.name) is not None:
			raise Exception(f"AddItem: Item with name {self.name} already exists in version {version.Name}. Cannot add a new item with the same name.")
		version.Items.append(
			Item( 
				name=self.name, 
				variables=self.Variables, 
				dependencies=self.Dependencies, 
				templates=self.Templates, 
				validations=self.Validations, 
				version=version, 
				previousVersion=None,
				isEntity=self.IsEntity,
				isModifiedFromPreviousVersion=False,
				isIdenticalToPreviousVersion=False,
			))

class AddEntity(AddItem):
	"""add a new entity to the package version"""
	def __init__(self, *, name:str, variables:list[Variable], dependencies:list[Dependency] = [], templates:list[Template] = [], validations:list[Validation] = [] ):
		super().__init__(name=name, variables=variables, dependencies=dependencies, templates=templates, validations=validations, isEntity=True)

class DeleteItem(Modification):
	"""delete an item from the package version"""
	def __init__(self, *, name:str):
		super().__init__(name=name)

	def Apply(self, version:Version) -> None:
		"""apply the modification to the version object, marking the item as deleted"""
		item = version.FindItem(self.name)
		if item is None:
			raise Exception(f"DeleteItem: Item with name {self.name} does not exist in version {version.Name}. Cannot delete a non-existing item.")
		if item.IsDeleted:
			raise Exception(f"DeleteItem: Item with name {self.name} is already deleted in version {version.Name}.")
		prevItem = item.PreviousVersion
		isEntity = item.IsEntity
		version.RemoveItem(self.name)
		version.Items.append(
			Item( 
				name=self.name, 
				isDeleted=True,
				previousVersion=prevItem,
				version=version,
				isEntity=isEntity
			))

# alias for DeleteItem, to make it more readable in the code		
DeleteEntity = DeleteItem

class ModifyItem(Modification):
	"""modify an item in the package version"""
	def __init__(self, *, 
			  name:str, 
			  addVariables:list[Variable] = [], 
			  removeVariables:list[str] = [], 
			  renameVariables:list[tuple[str, str]] = [],
			  addDependencies:list[Dependency] = [], 
			  removeDependencies:list[str] = [],
			  addTemplates:list[Template] = [], 
			  removeTemplates:list[str] = [],
			  addValidations:list[Validation] = [],
			  removeValidations:list[str] = [],
			  mappings:list[Mapping] = []
			  ):
		super().__init__(name=name)
		self.AddDependencies = addDependencies
		self.RemoveDependencies = removeDependencies
		self.AddTemplates = addTemplates
		self.RemoveTemplates = removeTemplates
		self.AddVariables = addVariables
		self.RemoveVariables = removeVariables
		self.RenameVariables = renameVariables
		self.AddValidations = addValidations
		self.RemoveValidations = removeValidations
		self.Mappings = mappings

	def Apply(self, version:Version) -> None:
		"""apply the modification to the version object, modifying the item in the version"""
		item = version.FindItem(self.name)
		if item is None:
			raise Exception(f"ModifyItem: Item with name {self.name} does not exist in version {version.Name}. Cannot modify a non-existing item.")
		if item.IsDeleted:
			raise Exception(f"ModifyItem: Item with name {self.name} is deleted in version {version.Name}. Cannot modify a deleted item.")
		if not item.IsIdenticalToPreviousVersion:
			raise Exception(f"ModifyItem: Item with name {self.name} must be identical from previous version  {version.Name} when applying ModifyItem.")

		# start by removing the existing item, and the adding a deep copy of the previous version of the item
		# to make sure we do not modify the previous version of the item
		version.RemoveItem(self.name)
		if item.PreviousVersion is None:
			raise Exception(f"ModifyItem: Item with name {self.name} does not have a previous version in version {version.Name}. Cannot modify an item without a previous version.")
		previousVersion = item.PreviousVersion
		item = copy.deepcopy(previousVersion)
		version.Items.append(item)

		def modifyList( originalList:list, addList:list, removeList:list[str] ) -> list:
			originalList.extend(addList)
			return [obj for obj in originalList if obj.Name not in removeList]

		item.IsIdenticalToPreviousVersion = False
		item.IsModifiedFromPreviousVersion = True
		item.PreviousVersion = previousVersion
		item.Dependencies = modifyList(item.Dependencies, self.AddDependencies, self.RemoveDependencies)
		item.Templates = modifyList(item.Templates, self.AddTemplates, self.RemoveTemplates)
		item.Validations = modifyList(item.Validations, self.AddValidations, self.RemoveValidations)
		item.Version = version

		# create a mapping list based on the existing variables, and add the modifications
		item.Mappings = []
		for variable in item.Variables:
			item.Mappings.append( Mapping(name=variable.Name, previousName=variable.Name ) )

		# add new variables
		for variable in self.AddVariables:
			if item.FindVariable(variable.Name) is not None:
				raise Exception(f"ModifyItem: Variable with name {variable.Name} already exists in item {item.Name} in version {version.Name}. Cannot add a variable with the same name.")
			item.Variables.append(variable)
			item.Mappings.append( Mapping(name=variable.Name, previousName=None) )
		
		# remove variables
		for variableName in self.RemoveVariables:
			variable = item.FindVariable(variableName)
			if variable is None:
				raise Exception(f"ModifyItem: Variable with name {variableName} does not exist in item {item.Name} in version {version.Name}. Cannot remove a non-existing variable.")
			item.Variables.remove(variable)

			# remove the mapping for the variable
			mapping = item.FindMapping(variableName)
			if mapping is None:
				raise Exception(f"ModifyItem: Mapping for variable with name {variableName} does not exist in item {item.Name} in version {version.Name}. Cannot remove a non-existing mapping.")
			mapping.IsDeleted = True

		# rename variables
		for oldName, newName in self.RenameVariables:
			variable = item.FindVariable(oldName)
			if variable is None:
				raise Exception(f"ModifyItem: Variable with name {oldName} does not exist in item {item.Name} in version {version.Name}. Cannot rename a non-existing variable.")
			variable.Name = newName

			# add a mapping for the renaming of the variable. find the mapping for the old name, if it exists
			mapping = item.FindMapping(oldName)
			if mapping is None:
				mapping = Mapping(name=newName, previousName=oldName)
				item.Mappings.append(mapping)
			else:
				mapping.Name = newName
				mapping.PreviousName = oldName
		
		# add or update mappings
		for mapping in self.Mappings:
			# remove the existing mapping if it has the same name
			existingMapping = item.FindMapping(mapping.Name)
			if existingMapping is not None:
				item.Mappings.remove(existingMapping)
			item.Mappings.append(mapping)

#alias for ModifyItem, to make it more readable in the code
ModifyEntity = ModifyItem

class Version:
	"""A specific version of a package, which defines all items which are available in that version, through modifications of a previous version"""

	# versionName - name of the version, use whatever convention you want, but needs to start with a letter, it will be the namespace of the version in the code
	# previousVersion - reference the version that this version is a direct update from
	# items - all the items in the version, including any item which is inherited from a previous version
	def __init__(self, name:str, previousVersion:Version, mods:list[Modification] ):
		self.Name = name

		# start with items which are identical to previous version' items
		self.Items = []
		if previousVersion != None:
			for prevItem in previousVersion.Items:
				if prevItem.IsDeleted:
					# if the previous item is deleted, we do not add it to this version
					continue
				self.Items.append(Item( 
					name=prevItem.Name, 
					previousVersion=prevItem, 
					version=self, 
					isEntity=prevItem.IsEntity,
					isIdenticalToPreviousVersion=True
					))
		self.PreviousVersion = previousVersion

		# apply modifications to the items in this version
		for mod in mods:
			mod.Apply(self)

	def FindItem( self, name ):
		if self.Items is None or len(self.Items) == 0:
			return None
		return next( (val for val in self.Items if val.Name == name), None )
	
	def RemoveItem( self, name ):
		if self.Items is None or len(self.Items) == 0:
			return None
		self.Items = [val for val in self.Items if val.Name != name]


class Package:
	"""The package of a project using pds"""

    # name - name of package (including any versioning)
    # parentPath - name of the folder where the package will be placed
	def __init__(self, name:str, path:str, versions:list[Version] ):
		self.Name = name
		self.Versions = versions
		self.Path = path
		self.SetupReferences()
		self.SetupPreviousVersionsOfItems()
		self.MakeSureAllItemsAreDefined()
		self.MakeSureAllVariablesAreMapped()

	# setup references to the package in all items and versions
	def SetupReferences(self) -> None:
		"""setup references to the package in all items and versions"""
		versionNames = [ver.Name for ver in self.Versions]
		if len(versionNames) != len(set(versionNames)):
			raise Exception(f"Package {self.Name} has duplicate version names: {versionNames}")
		for version in self.Versions:
			version.Package = self
			for item in version.Items:
				item.Package = self

	# for each version of the package, if it has a previous version
	# make sure all items in the previous version exists in this
	# version, unless it was deleted in the previous version
	def MakeSureAllItemsAreDefined(self) -> None:
		for version in self.Versions:
			prevVersion = version.PreviousVersion
			if prevVersion != None:
				for prevItem in prevVersion.Items:
					if not prevItem.IsDeleted:
						itmFnd = next( (itm for itm in version.Items if itm.Name == prevItem.Name ) , None )
						if itmFnd == None:
							raise Exception(f"Invalid setup in package {self.Name}, the item {prevItem.Name} in version {prevVersion.Name} is not defined in subsequent version {version.Name}")
						if itmFnd.IsEntity != prevItem.IsEntity:
							raise Exception(f"Invalid setup in package {self.Name}, the item {prevItem.Name} in version {prevVersion.Name} is not of the same Item/Entity type in subsequent version {version.Name}")

	# find all previous versions of items				
	def SetupPreviousVersionsOfItems(self) -> None:
		for version in self.Versions:
			for item in version.Items:
				if item.IsIdenticalToPreviousVersion or item.IsModifiedFromPreviousVersion:
					version = item.Version
					while version.PreviousVersion != None:
						version = version.PreviousVersion
						itmFnd = next( (itm for itm in version.Items if itm.Name == item.Name and not itm.IsIdenticalToPreviousVersion) , None )
						if itmFnd != None:
							if itmFnd.IsEntity != item.IsEntity:
								raise Exception(f"FindActualItem: In Package: {self.Name}, Version: {item.Version.Name} Found previous a item of the name '{item.Name}', but that item has IsEntity={itmFnd.IsEntity()} which does not match this item's IsEntity={itmFnd.IsEntity()}")
							item.PreviousVersion = itmFnd
							break
					if item.PreviousVersion == None:
						raise Exception(f"Invalid setup, the item {item.Name} in Package: {self.Name}, Version: {item.Version.Name} is not correctly setup, no previous version of the item is found in any package") 

	# for each item which is a modified version, make sure all variables are handled by mapping
	def MakeSureAllVariablesAreMapped(self) -> None:
		for version in self.Versions:
			for item in version.Items:
				if item.IsModifiedFromPreviousVersion:

					# collect all mappings, and check that all variables are covered
					coveredVariables = set()
					for mapping in item.Mappings:
						coveredVariables.add( mapping.Name )

					# make sure all variables are mapped in the set
					for variable in item.Variables:
						if variable.Name not in coveredVariables:
							raise Exception(f'Variable {variable.Name} is not handled by a mapping in version {version.Name} of item {item.Name}')
						
	def __str__(self):
		"""return a string representation of the package"""
		result = f'Package: {self.Name}, Path: {self.Path}\n'
		for version in self.Versions:
			result += f'  Version: {version.Name}\n'
			for item in version.Items:
				if item.IsDeleted:
					result += f'    Deleted Item: {item.Name}\n'
				elif item.IsIdenticalToPreviousVersion:
					result += f'    Identical Item: {item.Name}, Previous Version: {item.PreviousVersion.Version.Name}\n'
				else:
					result += f'    {"Modified" if item.IsModifiedFromPreviousVersion else "New" } Item: {item.Name}, IsEntity: {item.IsEntity}\n'
					for var in item.Variables:
						result += f'      Variable: {var.Name}, Type: {var.Type}, Optional: {var.Optional}, Vector: {var.Vector}, IndexedVector: {var.IndexedVector}\n'
		return result
	