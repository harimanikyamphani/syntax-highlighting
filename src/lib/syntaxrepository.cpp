/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "syntaxrepository.h"
#include "syntaxdefinition.h"

#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QStandardPaths>

using namespace SyntaxHighlighting;

SyntaxRepository::SyntaxRepository()
{
    load();
}

SyntaxRepository::~SyntaxRepository()
{
}

SyntaxDefinition SyntaxRepository::definitionForName(const QString& defName) const
{
    auto def = m_defs.value(defName);
    if (def.isValid())
        def.load();
    return def;
}

SyntaxDefinition SyntaxRepository::definitionForFileName(const QString& fileName) const
{
    QFileInfo fi(fileName);
    const auto ext = fi.suffix();

    for (auto it = m_defs.constBegin(); it != m_defs.constEnd(); ++it) {
        auto def = it.value();
        if (def.extensions().contains(ext)) {
            def.load();
            return def;
        }
    }

    return SyntaxDefinition();
}

QVector<SyntaxDefinition> SyntaxRepository::definitions() const
{
    QVector<SyntaxDefinition> defs;
    defs.reserve(m_defs.size());
    for (auto it = m_defs.constBegin(); it != m_defs.constEnd(); ++it)
        defs.push_back(it.value());
    return defs;
}

void SyntaxRepository::load()
{
    foreach (const auto &dir, QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)) {
        loadFolder(dir + QStringLiteral("/KateSyntax"));
    }
    loadFolder(QStringLiteral(":/syntaxhighlighting/syntax"));
}

void SyntaxRepository::loadFolder(const QString &path)
{
    QDirIterator it(path);
    while (it.hasNext()) {
        SyntaxDefinition def;
        def.setSyntaxRepository(this);
        if (def.loadMetaData(it.next()))
            addDefinition(def);
    }
}

void SyntaxRepository::addDefinition(const SyntaxDefinition &def)
{
    const auto it = m_defs.constFind(def.name());
    if (it == m_defs.constEnd()) {
        m_defs.insert(def.name(), def);
        return;
    }

    if (it.value().version() >= def.version())
        return;
    m_defs.insert(def.name(), def);
}
