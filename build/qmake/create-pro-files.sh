#! /bin/bash -e

for d in tests examples tools-dev; do
  for i in ../../"${d}"/*.cpp ; do
    if [ -f "${i}" ]; then
        b=$(basename ${i%.cpp})
        mkdir -p "${b}"
        sed "s/{{{1}}}/${b}/g;s/{{{2}}}/${d}/g" <pro >"${b}/${b}.pro"
    fi
  done
done

