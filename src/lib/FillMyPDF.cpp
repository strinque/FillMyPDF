#include "FillMyPDF.h"
#pragma warning( push )
#pragma warning( disable: 4996 )
  #include <podofo/podofo.h>
#pragma warning( pop )
using namespace PoDoFo;

// separate implementation from the interface
class FillMyPDFImpl final
{
public:
  FillMyPDFImpl() :
    m_pdf(),
    m_fields()
  {
  }

  // load and parse pdf document
  void load(const std::filesystem::path& path)
  {
    // check that file exists
    if (!std::filesystem::exists(path))
      throw std::exception("file doesn't exists");

    // load pdf document from file
    PoDoFo::PdfError::EnableDebug(false);
    m_pdf.Load(path.string().c_str());
    if (!m_pdf.IsLoaded())
      throw std::exception("can't load the document");

    // force the appearance of fields
    m_pdf.GetAcroForm()->SetNeedAppearances(true);

    // extract all fields from document
    for (int i = 0; i < m_pdf.GetPageCount(); ++i)
    {
      PdfPage* pPage = m_pdf.GetPage(i);
      for (int f = 0; f < pPage->GetNumFields(); ++f)
        m_fields.push_back(pPage->GetField(f));
    }
  }

  // save pdf into file
  void save(const std::filesystem::path& path)
  {
    m_pdf.Write(path.string().c_str());
  }

  // fill pdf with data (text and checkbox)
  void fill(const std::map<std::string, std::string>& texts,
            const std::map<std::string, bool>& checkboxes)
  {
    // fill each supported fields
    for (const auto& f : m_fields)
    {
      switch (f.GetType())
      {
      case ePdfField_TextField:
      {
        // check if this field needs to be filled
        const auto& it = texts.find(f.GetFieldName().GetString());
        if (it != texts.end())
        {
          // set field text content
          PdfTextField text(f);
          text.SetText(it->second);
        }
        break;
      }
      case ePdfField_CheckBox:
      {
        // check if this checkbox has to be filled
        const auto& it = checkboxes.find(f.GetFieldName().GetString());
        if (it != checkboxes.end())
        {
          PdfCheckBox checkbox(f);
          checkbox.SetChecked(it->second);
        }
        break;
      }
      case ePdfField_PushButton:
      case ePdfField_RadioButton:
      case ePdfField_ComboBox:
      case ePdfField_ListBox:
      case ePdfField_Signature:
      case ePdfField_Unknown:
      default:
        break;
      }
    }
  }

private:
  PoDoFo::PdfMemDocument m_pdf;
  std::vector<PoDoFo::PdfField> m_fields;
};

// constructor
FillMyPDF::FillMyPDF() :
  m_pimpl(std::make_unique<FillMyPDFImpl>())
{
}

// destructor
FillMyPDF::~FillMyPDF() = default;

// load pdf file
void FillMyPDF::load(const std::filesystem::path& path)
{
  if (m_pimpl)
    m_pimpl->load(path);
}

// fill pdf with data (text and checkbox)
void FillMyPDF::fill(const std::map<std::string, std::string>& texts,
                     const std::map<std::string, bool>& checkboxes)
{
  if (m_pimpl)
    m_pimpl->fill(texts, checkboxes);
}

// save pdf into file
void FillMyPDF::save(const std::filesystem::path& path)
{
  if (m_pimpl)
    m_pimpl->save(path);
}