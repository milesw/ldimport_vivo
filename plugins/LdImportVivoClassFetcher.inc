<?php

/**
 * @file
 * Class definition for Linked Data Import's fetcher plugins.
 */

/**
 * Class definition for LdImportVivoClassFetcher.
 */
class LdImportVivoClassFetcher extends LdImportFetcher {

  /**
   * Gets the list of individual URIs for a particular class.
   */
  public function getURIs($source) {
    $source_config = $source->getConfigFor($this);
    $vivo_url = $source_config['site'];
    $class_uri = $source_config['class'];
    $uri_list = array();

    if (empty($vivo_url) || empty($class_uri)) {
      return $uri_list;
    }

    // This is the URL used for the RDF links on VIVO index pages.
    $target_url = url($vivo_url . '/listrdf', array('query' => array('vclass' => $class_uri)));

    $parser = ARC2::getRDFParser();
    $parser->parse($target_url);

    foreach ($parser->getTriples() as $triple) {
      if ($triple['o'] == $class_uri && $triple['p'] == 'http://www.w3.org/1999/02/22-rdf-syntax-ns#type') {
        $uri = $triple['s'];
        $uri_list[$uri] = $uri;
      }
    }

    return array_values($uri_list);
  }

  /**
   * Override parent::sourceForm().
   */
  public function sourceForm($source_config) {
    $form['site'] = array(
      '#type' => 'textfield',
      '#title' => t('VIVO Site URL'),
      '#description' => t('Enter the base URL for the VIVO site without a trailing slash (e.g. http://vivo.cornell.edu)'),
      '#default_value' => isset($source_config['site']) ? $source_config['site'] : '',
    );
    $form['class'] = array(
      '#type' => 'textfield',
      '#title' => t('VIVO Class URI'),
      '#description' => t('Enter the full URI for the class of individuals to import.'),
      '#default_value' => isset($source_config['class']) ? $source_config['class'] : '',
    );
    return $form;
  }

  /**
   * Override parent::sourceForm().
   */
  public function sourceFormValidate(&$values) {
    if (empty($values['site']) || empty($values['class'])) {
      return;
    }
    if (!valid_url($values['site'])) {
      form_set_error('feeds][site', t('The URL "%site" is not valid.', array('%site' => $values['site'])));
    }
    if (!valid_url($values['class'], TRUE)) {
      form_set_error('feeds][class', t('The URI "%class" is not valid.', array('%class' => $values['class'])));
    }
  }
}
