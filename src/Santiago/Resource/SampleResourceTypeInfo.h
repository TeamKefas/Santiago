#ifndef SAMPLERESOURCETYPEINFO_H
#define SAMPLERESOURCETYPEINFO_H

namespace UserNamespace
{
    struct MyResourceTypesInfo
    {
        enum ResourceTypeEnum
        {
            USER_INFO = 0,
            TEXT_DOCUMENT = 1,
            EXCEL_SHEET = 2
        };
        
        typedef boost::mpl::vector<UserInfo,TextDocument,ExcelSheet> ResourceTypeList;
        static constexpr std::array<std::string,3> ResourceLabelList = 
        {
            "UserInfo",
            "TextDocument",
            "ExcelSheet"
        };
        
    };


}


#endif
