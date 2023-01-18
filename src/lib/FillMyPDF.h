#pragma once
#include <map>
#include <string>
#include <filesystem>
#include <memory>
#include <stdbool.h>

class FillMyPDFImpl;
class FillMyPDF final
{
  // delete copy/assignement operators
  FillMyPDF(const FillMyPDF&) = delete;
  FillMyPDF& operator=(const FillMyPDF&) = delete;
  FillMyPDF(FillMyPDF&&) = delete;
  FillMyPDF& operator=(FillMyPDF&&) = delete;

public:
  // constructor/destructor
  FillMyPDF();    
  ~FillMyPDF();

  // load pdf file
  void load(const std::filesystem::path& path);

  // fill pdf with data (text and checkbox)
  void fill(const std::map<std::string, std::string>& texts,
            const std::map<std::string, bool>& checkboxes = {});

  // save pdf into file
  void save(const std::filesystem::path& path);

private:
  // pointer to internal implementation
  std::unique_ptr<FillMyPDFImpl> m_pimpl;
};