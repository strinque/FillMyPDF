#include "FillMyPDF.h"
#include <podofo/podofo.h>
using namespace PoDoFo;

// separate implementation from the interface
class FillMyPDFImpl final
{
public:
  FillMyPDFImpl() :
    m_pdf()
  {
  }

  // load and parse pdf document
  void load(const std::filesystem::path& path)
  {
    // check that file exists
    if (!std::filesystem::exists(path))
      throw std::exception("file doesn't exists");

    // disable PoDoFo logging system
    PoDoFo::PdfCommon::SetMaxLoggingSeverity(PoDoFo::PdfLogSeverity::None);

    // load pdf document from file
    m_pdf.Load(path.string().c_str());

    // force the appearance of fields
    m_pdf.GetAcroForm()->SetNeedAppearances(true);
  }

  // save pdf into file
  void save(const std::filesystem::path& path)
  {
    m_pdf.Save(path.string().c_str());
  }

  // fill pdf with data (text and checkbox)
  void fill(const std::map<std::string, std::string>& texts,
            const std::map<std::string, bool>& checkboxes)
  {
    // fill each supported fields
    for (int i = 0; i < m_pdf.GetAcroForm()->GetFieldCount(); ++i)
    {
      PdfField& field = m_pdf.GetAcroForm()->GetFieldAt(i);
      if (!field.GetName().has_value())
        continue;
      const std::string& field_name = field.GetName()->GetString();
      switch (field.GetType())
      {
      case PdfFieldType::TextBox:
      {
        // check if this field needs to be filled
        const auto& it = texts.find(field_name);
        if (it != texts.end())
        {
          PdfTextBox* textbox = dynamic_cast<PdfTextBox*>(&field);
          if (!textbox)
            throw std::runtime_error("can't replace PdfTextBox field: " + field_name);
          textbox->SetText(PdfString(it->second));
        }
        break;
      }
      case PdfFieldType::CheckBox:
      {
        // check if this checkbox has to be filled
        const auto& it = checkboxes.find(field_name);
        if (it != checkboxes.end())
        {
          PdfCheckBox* checkbox = dynamic_cast<PdfCheckBox*>(&field);
          if(!checkbox)
            throw std::runtime_error("can't replace PdfCheckBox field: " + field_name);
          checkbox->SetChecked(it->second);
        }
        break;
      }
      default:
        break;
      }
    }
  }

private:
  PdfMemDocument m_pdf;
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