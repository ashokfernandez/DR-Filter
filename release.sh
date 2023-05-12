#!/bin/bash

# get latest tag
latest=$(git describe --tags `git rev-list --tags --max-count=1`)
latest=${latest#release-}  # remove the 'release-' prefix

# split into array
ver=($(echo $latest | tr "." "\n"))

# get version numbers
major=${ver[0]}
minor=${ver[1]}
patch=${ver[2]}

echo "Current version : $major.$minor.$patch"

# Ask user for the new version
read -p "Enter the version to increment (major/minor/patch): " version

case $version in
  "major")
    major=$((major + 1))
    minor=0  # reset minor and patch version when major version is incremented
    patch=0
    ;;
  "minor")
    minor=$((minor + 1))
    patch=0  # reset patch version when minor version is incremented
    ;;
  "patch")
    patch=$((patch + 1))
    ;;
  *)
    echo "Invalid version type."
    exit 1
    ;;
esac

# create new tag
newTag="release-$major.$minor.$patch"

# confirm with the user
while true; do
    read -p "Create new tag $newTag? (y/n) " yn
    case $yn in
        [Yy]* ) git tag $newTag; git push origin $newTag; break;;
        [Nn]* ) echo "Update failed. Exiting..."; exit;;
        * ) echo "Please answer yes (y) or no (n).";;
    esac
done
