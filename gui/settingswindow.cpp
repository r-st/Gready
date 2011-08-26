/*
    Copyright (c) 2011 Robert Štětka <robert.stetka@gmail.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/


#include "settingswindow.h"

SettingsWindow::SettingsWindow()
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  QFormLayout* formLayout = new QFormLayout;
  QSettings settings;
  
  //layout->setMinimumWidth(50);
  
  m_usernameLineEdit = new QLineEdit;
  m_usernameLineEdit->setText(settings.value("username").toString());
  formLayout->addRow(tr("Login:"), m_usernameLineEdit);
  
  m_passwordLineEdit = new QLineEdit;
  m_passwordLineEdit->setEchoMode(QLineEdit::Password);
  m_passwordLineEdit->setText(settings.value("password").toString());
  formLayout->addRow(tr("Password"), m_passwordLineEdit);
  
  layout->addLayout(formLayout);
  
  QDialogButtonBox* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(btnBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
  connect(btnBox, SIGNAL(rejected()), SLOT(close()));
  
  layout->addWidget(btnBox);
  setLayout(layout);
}

void SettingsWindow::saveSettings()
{
  QSettings settings;
  if(m_usernameLineEdit->text() != settings.value("username")) {    
    settings.setValue("username", m_usernameLineEdit->text());
  }
  
  if(m_passwordLineEdit->text() != settings.value("password")) {
    settings.setValue("password", m_passwordLineEdit->text());
  }
  close();
}

#include "settingswindow.moc"

