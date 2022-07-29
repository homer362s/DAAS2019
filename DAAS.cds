<distribution version="19.0.0" name="DAAS" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{F5E45E74-B19D-43C1-A1C6-3576D39E1CDC}">
		<general appName="DAAS" outputLocation="c:\Users\user\Desktop\DAAS2019\cvidistkit.DAAS" relOutputLocation="cvidistkit.DAAS" outputLocationWithVars="c:\Users\user\Desktop\DAAS2019\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.1">
			<arp company="" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" showPaths="true" showRuntimeOnly="true" readMe="" license="">
			<dlgstrings welcomeTitle="DAAS" welcomeText=""/></userinterface>
		<dirs appDirID="101">
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="DAAS" dirID="100" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="DAAS" dirID="101" parentID="2" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\Users\user\Desktop\DAAS2019\cvibuild.DAAS\Release\DAAS.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="C:\Program Files (x86)\Measurement Computing\DAQ\cbw32.dll" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="101" projectID="0">
				<fileID>0</fileID></projectOutput>
			<projectDependencies dirID="101" projectID="0">
				<fileID>1</fileID></projectDependencies></fileGroups>
		<shortcuts>
			<shortcut name="DAAS" targetFileID="0" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<runtimeEngine installToAppDir="false" activeXsup="false" analysis="true" cvirte="true" dotnetsup="true" instrsup="true" lowlevelsup="true" lvrt="true" netvarsup="true" rtutilsup="true">
			<hasSoftDeps/>
			<doNotAutoSelect>
			<component>activeXsup</component>
			</doNotAutoSelect></runtimeEngine><sxsRuntimeEngine>
			<selected>false</selected>
			<doNotAutoSelect>false</doNotAutoSelect></sxsRuntimeEngine>
		<advanced mediaSize="650">
			<exeName>install.exe</exeName>
			<launchConditions>
				<condition>MINOS_WIN7_SP1</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<baselineProducts>
			<product name="NI LabWindows/CVI Shared Runtime 2020" UC="{80D3D303-75B9-4607-9312-E5FC68E5BFD2}" productID="{D1AB9946-A95B-408F-BEC5-DA8E87D6101E}" path="(None)" flavorID="_full_" flavorName="Full" verRestr="false" coreVer="20.0.49152">
				<dependencies>
					<productID>{07298686-C518-4981-9AE1-6E62273BF43A}</productID>
					<productID>{0FFFDCED-4232-4312-BBDB-179F9FDAB7EA}</productID>
					<productID>{10C707D2-DE6B-4908-8FD5-C3ACC763C12C}</productID>
					<productID>{192594FE-5397-4F45-8137-A5C487CE3849}</productID>
					<productID>{1C79B8BD-F5F8-4825-8BC8-78098A726AE1}</productID>
					<productID>{28B285EA-90C8-4C3A-94C0-B0196A3FEF07}</productID>
					<productID>{39CB13EF-8677-4FF8-942D-20021AFD7AB6}</productID>
					<productID>{3F36AC37-CC28-4780-AF19-D80ACEDE0323}</productID>
					<productID>{41BDEA25-514E-4BDA-AC47-186E97C9D3F7}</productID>
					<productID>{43C6873A-8548-4944-AF22-8B1C3312A070}</productID>
					<productID>{4C351C60-5ECC-4920-AEB6-DFE60EA14283}</productID>
					<productID>{539F08CF-F2FD-482C-A5ED-65FD2620789D}</productID>
					<productID>{62418E99-C730-4D5D-96B2-B4DDB810A652}</productID>
					<productID>{64ECB814-3A6A-4E48-9D2F-D6C2EDD725B7}</productID>
					<productID>{655826D6-553F-4DD9-A3D4-BA853FC88F77}</productID>
					<productID>{67CFC7DD-BF77-4D45-9AE9-742AA8658AE7}</productID>
					<productID>{69586F66-56DA-43BD-BB8E-A6DAE9A4DF6D}</productID>
					<productID>{7B66645D-2C18-4DB8-8D3B-A783E386708E}</productID>
					<productID>{866E5D39-AB76-4536-B1CA-554BC66A1900}</productID>
					<productID>{88DFF01E-7C87-4E35-B194-CB7A2E32FA6D}</productID>
					<productID>{9905478F-5785-468A-A35A-D84325D61EB8}</productID>
					<productID>{A12FB799-41BF-4A9A-98F2-2FE4F333B718}</productID>
					<productID>{A59D9F6E-27DE-410C-9820-9D844DC78D05}</productID>
					<productID>{AFC999BB-F270-46EF-B748-AE755EC75322}</productID>
					<productID>{B524884D-C3AC-417A-939E-34A16118A14C}</productID>
					<productID>{C0A9F5E2-DCD5-44C1-8B03-C560F4C06D6C}</productID>
					<productID>{C1771155-665A-4C7F-B85D-D4AACF786223}</productID>
					<productID>{C18DA001-CF46-48A5-8DDB-322EA6DBFB4F}</productID>
					<productID>{D8F71C3E-C0AE-4D69-920E-7D927C950A1A}</productID>
					<productID>{DCCB918E-584B-4FAF-BDA4-EF5A7B90C4C1}</productID>
					<productID>{E0FBECD8-D116-47B8-8839-AA2A039EBD46}</productID>
					<productID>{EDEDDCAE-21E6-4A82-9B58-8E6337386685}</productID>
					<productID>{EFC648C5-F3BC-4096-9AFE-23121EF06828}</productID>
					<productID>{F2EEE63A-0DCA-41BF-A243-4E4C0DFA38A4}</productID></dependencies></product>
			<product name="NI-488.2 Runtime 20.0" UC="{357F6618-C660-41A2-A185-5578CC876D1D}" productID="{CB6C2533-4926-42B8-AC21-04BB9679F818}" path="(None)" flavorID="_full_" flavorName="NI-488.2" verRestr="false" coreVer="20.0.49152">
				<dependencies>
					<productID>{07298686-C518-4981-9AE1-6E62273BF43A}</productID>
					<productID>{0FCD10E5-84C0-4A16-811D-B5466741E8CB}</productID>
					<productID>{0FFFDCED-4232-4312-BBDB-179F9FDAB7EA}</productID>
					<productID>{139B293A-1226-4199-AC8D-A238ADC86EB9}</productID>
					<productID>{145CF2A0-2473-4820-BE66-15C4BD73E366}</productID>
					<productID>{1C79B8BD-F5F8-4825-8BC8-78098A726AE1}</productID>
					<productID>{23970F58-0D11-4450-8E80-3CAD6FD4EF04}</productID>
					<productID>{272F9AE4-F151-4D09-9F94-06829C2824F5}</productID>
					<productID>{27F3F44F-D746-4FAE-A996-B470E6DD6C1A}</productID>
					<productID>{2974A7A1-A67A-40D8-A3C1-70063CD536BC}</productID>
					<productID>{3BD72613-A41E-48BD-A399-0F6C85DB259F}</productID>
					<productID>{51EB7A75-908C-481F-A2A7-48AAD15B3DFD}</productID>
					<productID>{5E39EF13-6B0A-4C7A-B7FB-608DB58D1D73}</productID>
					<productID>{62418E99-C730-4D5D-96B2-B4DDB810A652}</productID>
					<productID>{64ECB814-3A6A-4E48-9D2F-D6C2EDD725B7}</productID>
					<productID>{69586F66-56DA-43BD-BB8E-A6DAE9A4DF6D}</productID>
					<productID>{73689806-52A9-46B8-B604-6EA7252CDF3F}</productID>
					<productID>{7B66645D-2C18-4DB8-8D3B-A783E386708E}</productID>
					<productID>{7FCBF80E-B8DE-4A3C-A750-0088A9314E8C}</productID>
					<productID>{7FD3AEFB-6522-41BE-B449-899C5493D849}</productID>
					<productID>{85BC3932-0E03-40D3-AD54-C87505762B2E}</productID>
					<productID>{866E5D39-AB76-4536-B1CA-554BC66A1900}</productID>
					<productID>{8F7F8C7A-D00C-417C-B053-E0035F1E9B58}</productID>
					<productID>{9EDA1388-B38D-43F7-99D1-8D41E2AE983B}</productID>
					<productID>{A1A16811-5E1A-4F4F-A4EE-B1E8A5780C87}</productID>
					<productID>{AD73E3BB-F089-4B66-9A68-E7BA322B1CED}</productID>
					<productID>{AFC999BB-F270-46EF-B748-AE755EC75322}</productID>
					<productID>{B13F7BB7-FD5C-4B1C-AF6D-BD81A2C17041}</productID>
					<productID>{BC7C02E4-C277-4FDB-8709-2C68334BCB2D}</productID>
					<productID>{C1771155-665A-4C7F-B85D-D4AACF786223}</productID>
					<productID>{CDB47FFA-0760-4888-A5D2-54CDA3C8C29E}</productID>
					<productID>{D7C0F171-E494-4115-9F27-8B2B9ECADAB4}</productID>
					<productID>{DD0095FB-2C15-49CC-AAB6-14C0E0B93794}</productID>
					<productID>{EE79EDE0-8906-44DC-B644-595246FB8ACD}</productID>
					<productID>{EFC648C5-F3BC-4096-9AFE-23121EF06828}</productID>
					<productID>{F3629CA4-ADAE-4A1C-BDA6-AAF4AB3BBE1A}</productID>
					<productID>{FB7B16DB-F395-4D41-A557-80386E0D771E}</productID>
					<productID>{FC9981A5-0F21-4461-913F-0EEE451304E8}</productID></dependencies></product></baselineProducts>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\Users\user\Desktop\DAAS2019\DAAS.prj" ProjectRelativePath="DAAS.prj"/></Projects>
		<buildData progressBarRate="0.441514584079939">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.248508000000000</ProductsAdded>
				<DPConfigured>4.198510500000000</DPConfigured>
				<DPMergeModulesAdded>6.658510500000000</DPMergeModulesAdded>
				<DPClosed>14.483510999999998</DPClosed>
				<DistributionsCopied>24.483764999999998</DistributionsCopied>
				<End>226.493084499999981</End></progressTimes></buildData>
	</msi>
</distribution>
